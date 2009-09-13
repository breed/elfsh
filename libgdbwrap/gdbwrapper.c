/**
* @file libgdbwrap/gdbwrapper.c
 * @brief Implements the GDB serial protocol.
 * @ingroup libgdbwrap
 *
 * $Id$
 */

/*
 *  See gdb documentation, section D for more information on the
 *  remote serial protocol. To make it short, a packet looks like the following:
 *
 *  $packet-data#checksum  or  $sequence-id:packet-data#checksum.
 *
 *  where the checksum is the sum of all the characters modulo 256.
 */

#include             <netdb.h>
#include             "libaspect.h"
#include             "gdbwrapper-internals.h"
#include             "gdbwrapper.h"

gdbwrapworld_t       gdbwrapworld;

/******************** Internal functions ********************/

static char          *gdbwrap_lastmsg(gdbwrap_t *desc)
{
  return desc->packet;
}


static Bool          gdbwrap_errorhandler(gdbwrap_t *desc, const char *error)
{
  ASSERT(desc != NULL && error != NULL);

  DEBUGMSG(printf("Treating error (encoded): %s\n", error));

  if (!strncmp(GDBWRAP_REPLAY_OK, error, strlen(GDBWRAP_REPLAY_OK)))
    {
      desc->erroroccured = FALSE;
      return FALSE;
    }

  if (!strncmp(GDBWRAP_NO_SPACE, error, strlen(GDBWRAP_NO_SPACE)))
    fprintf(stderr, "space was not updated.\n");

  if (!strncmp(GDBWRAP_NO_TABLE, error, strlen(GDBWRAP_NO_TABLE)))
    fprintf(stdout, "Not populating the table\n");

  if (!strncmp(GDBWRAP_DEAD, error, strlen(GDBWRAP_DEAD)))
    fprintf(stdout, "The server seems to be dead. Message not sent.\n");

  if (error[0] == GDBWRAP_REPLAY_ERROR)
    fprintf(stdout, "Error received from the server: %s\n", error);

  if (error[0] == GDBWRAP_EXIT_W_STATUS)
    {
      fprintf(stdout, "Exit with status: %s\n", error);
      desc->is_active = FALSE;
    }

  if (error[0] == GDBWRAP_EXIT_W_SIGNAL)
    {
      fprintf(stdout, "Exit with signal: %s\n", error);
      desc->is_active = FALSE;
    }

  desc->erroroccured = TRUE;
  fflush(stdout);
  return TRUE;
}


static Bool         gdbwrap_is_interrupted(gdbwrap_t *desc)
{
  return desc->interrupted;
}


/**
 * This function parses a string *strtoparse* starting at character
 * *begin* and ending at character *end*. The new parsed string is
 * saved in *strret*. If *begin* is not found in *strtoparse* then the
 * function returns NULL. If *end* is not found in *strtoparse*, then
 * the function returns NULL..
 *
 * @param strtoparse: String to parse.
 * @param strret    : String to return without *begin* and *end*.
 * @param begin     : String where to start parsing. If NULL,
 *                    we start from the beginning.
 * @param end       : String where to end parsing. If NULL,
 *                    we copy the string from *begin* to then
 *                    end of *strtoparse* (ie a NULL char is found).
 * @param maxsize   : The maximum size to extract.
 * @return          : Returns a pointer on the beginning of the new string.
 */
static char          *gdbwrap_extract_from_packet(const char *strtoparse,
						  char *strret,
						  const char *begin,
						  const char *end,
                                                  int maxsize)
{
  const char         *charbegin;
  const char         *charend;
  unsigned           strtorem;
  ptrdiff_t          strsize;

  ASSERT(strtoparse != NULL);

  if (begin == NULL)
    {
      charbegin = strtoparse;
      strtorem  = 0;
    }
  else
    {
      charbegin = strstr(strtoparse, begin);
      strtorem  = strlen(begin);
      if (charbegin == NULL)
	return NULL;
    }

  if (end == NULL)
    charend = charbegin + strlen(charbegin);
  else
    {
      u_int off = (begin == NULL) ? 0 : strlen(begin);
      
      charend = strstr(charbegin + off, end);
      if (charend == NULL)
	return NULL;
    }

  strsize = charend - charbegin - strtorem;
  if (strsize > maxsize)
    strsize = maxsize;

  strncpy(strret, charbegin + strtorem, strsize);
  strret[strsize] = GDBWRAP_NULL_CHAR;

  return strret;
}

static la32          gdbwrap_little_endian(la32 addr)
{
  la32               addrlittle = 0;
  unsigned           i;

  for (i = 0; addr > 0; i++)
    {
      addrlittle += (LOBYTE(addr) << (BYTE_IN_BIT * (sizeof(addr) - 1 - i)));
      addr >>= BYTE_IN_BIT;
    }

  return addrlittle;
}


static uint8_t       gdbwrap_calc_checksum(gdbwrap_t *desc, const char *str)
{
  unsigned           i;
  uint8_t            sum;
  char              *result;

  result = gdbwrap_extract_from_packet(str, desc->packet, GDBWRAP_BEGIN_PACKET,
                                       GDBWRAP_END_PACKET,
				       desc->max_packet_size);

  /* If result == NULL, it's not a packet. */
  if (result == NULL)
    result = gdbwrap_extract_from_packet(str, desc->packet, NULL, NULL,
					 desc->max_packet_size);

  for (i = 0, sum = 0; i < strlen(result); i++)
    sum += result[i];

  return  sum;
}


static char          *gdbwrap_make_message(gdbwrap_t *desc, const char *query)
{
  uint8_t             checksum       = gdbwrap_calc_checksum(desc, query);
  unsigned            max_query_size = (desc->max_packet_size -
					strlen(GDBWRAP_BEGIN_PACKET)
					- strlen(GDBWRAP_END_PACKET)
					- sizeof(checksum));

  /* Sometimes C sucks... Basic source and destination checking. We do
     not check the overlapping tho.*/
  if (strlen(query) < max_query_size && query != desc->packet)
    {
      u_char ret;
      ret = snprintf(desc->packet, desc->max_packet_size, "%s%s%s%.2x",
		     GDBWRAP_BEGIN_PACKET, query, GDBWRAP_END_PACKET, checksum);
      ASSERT(ret > 0);
    }
  else
    {
      ASSERT(FALSE);
    }

  return desc->packet;
}


/**
 * This function performes a run-length decoding and writes back to
 * *dstpacket*, but no more than *maxsize* bytes.
 *
 * @param srcpacket: the encoded packet.
 * @param maxsize:   the maximal size of the decoded packet.
 */
static char          *gdbwrap_run_length_decode(char *dstpacket, const char *srcpacket,
						unsigned maxsize)
{
  /* Protocol specifies to take the following value and substract 29
     and repeat by this number the previous character.  Note that the
     compression may be used multiple times in a packet. */
  char              *encodestr;
  char               valuetocopy;
  uint8_t            numberoftimes;
  unsigned           iter;
  unsigned           strlenc;
  unsigned           check;

  ASSERT(dstpacket != NULL && srcpacket != NULL &&
  	 srcpacket[0] != GDBWRAP_START_ENCODC);
  if (srcpacket != dstpacket)
    strncpy(dstpacket, srcpacket, maxsize);
  encodestr = strstr(dstpacket, GDBWRAP_START_ENCOD);
  check = strlen(dstpacket);
  while (encodestr != NULL)
    {
      /* This    is    OK   to    take    encodestr[-1],   since    we
	 assert(srcpacket[0] != GDBWRAP_START_ENCODC). */
      valuetocopy    = encodestr[-1];
      numberoftimes  = encodestr[1] - 29;
      ASSERT((check += numberoftimes) < maxsize);
      strlenc        = strlen(encodestr);
      /* We move the string to the right, then set the bytes. We
	 substract 2, because we have <number>*<char> where * and
	 <char> are filled with the value of <number> (ie 2 chars). */
      for (iter = 0; iter < strlenc; iter++)
	encodestr[strlenc + numberoftimes - iter - 2] = encodestr[strlenc - iter];
      memset(encodestr, valuetocopy, numberoftimes);
      encodestr = strstr(NEXT_CHAR(encodestr), GDBWRAP_START_ENCOD);
    }

  return dstpacket;
}


/**
 * Populate the gdb registers with the values received in the
 * packet. A packet has the following form:
 *
 * $n:r;[n:r;]#checksum
 *
 * where n can be a number (the register), or "thread" and r is the
 * value of the thread/register.
 *
 * @param packet: the packet to parse.
 * @param reg   : the structure in which we want to write the registers.
 */
static void         gdbwrap_populate_reg(gdbwrap_t *desc, char *packet)
{
  const char       *nextpacket;
  char             *nextupacket;
  char              packetsemicolon[MSG_BUF];
  char              packetcolon[MSG_BUF];
  unsigned          packetoffset = 0;

  /* If a signal is received, we populate the registers, starting
     after the signal number (ie after Tnn, where nn is the
     number). */
  if (packet[0] == GDBWRAP_SIGNAL_RECV)
    packetoffset = 3;

  while ((nextpacket =
	  gdbwrap_extract_from_packet(packet + packetoffset,
				      packetsemicolon,
				      NULL,
				      GDBWRAP_SEP_SEMICOLON,
				      sizeof(packetsemicolon))) != NULL)
    {
      nextupacket = gdbwrap_extract_from_packet(nextpacket, packetcolon, NULL,
						GDBWRAP_SEP_COLON,
						sizeof(packetcolon));
      ASSERT(nextupacket != NULL);
      if (strlen(nextupacket) == 2)
	{
	  uint8_t regnumber = gdbwrap_atoh(nextupacket, strlen(nextupacket));
	  ureg32  regvalue;

	  nextupacket = gdbwrap_extract_from_packet(nextpacket, packetcolon,
						    GDBWRAP_SEP_COLON, NULL,
						    sizeof(packetcolon));
	  ASSERT(nextupacket != NULL);
	  regvalue = gdbwrap_atoh(nextupacket, strlen(nextupacket));
	  regvalue = gdbwrap_little_endian(regvalue);

	  *(&desc->reg32.eax + regnumber) =  regvalue;
	}
      /* We add 1 in order not to take the right limit. In the worst
	 case, we should get the NULL char. */
      packetoffset += strlen(nextpacket) + 1;
    }
}


static void          gdbwrap_send_ack(gdbwrap_t *desc)
{
  send(desc->fd, GDBWRAP_COR_CHECKSUM, strlen(GDBWRAP_COR_CHECKSUM), 0x0);
}


static Bool          gdbwrap_check_ack(gdbwrap_t *desc)
{
  int                rval;

  rval = recv(desc->fd, desc->packet, 1, 0);
  /* The result of the previous recv must be a "+". */
  if (!rval)
    desc->is_active = FALSE;
  if (desc->packet[0] == GDBWRAP_COR_CHECKSUMC && rval != -1) 
    return TRUE;
  else
    if (desc->packet[0] != GDBWRAP_BAD_CHECKSUM)
      return FALSE;
    else
      {
	fprintf(stderr, "The server has NOT sent any ACK."
		"It probably does not follow exactly the gdb protocol (%s - %d).\n",
		desc->packet, rval);
	return FALSE;
      }
}


static char          *gdbwrap_get_packet(gdbwrap_t *desc)
{
  int                 rval;
  int                 sumrval;
  char                checksum[3];

  ASSERT(desc != NULL);

  desc->packet[0] = GDBWRAP_NULL_CHAR;
  rval = -1;
  sumrval = 0;
  do
    {
      /* In case the packet is splitted into many others. */
      rval = recv(desc->fd, desc->packet + sumrval, desc->max_packet_size, 0);
      sumrval += rval;
      if (errno == EINTR)
	break;
    } while (sumrval >= 3 &&
	     desc->packet[sumrval - 3] != GDBWRAP_END_PACKETC && rval);
      

  /* if rval == 0, it means the host is disconnected/dead. */
  if (rval)
    {
      desc->packet[sumrval] = GDBWRAP_NULL_CHAR;
      gdbwrap_extract_from_packet(desc->packet, checksum, GDBWRAP_END_PACKET,
				  NULL, sizeof(checksum));
      /* If no error, we ack the packet. */
      if (rval != -1 &&
	  gdbwrap_atoh(checksum, strlen(checksum)) ==
	  gdbwrap_calc_checksum(desc, desc->packet))
	{
	  gdbwrap_send_ack(desc);
	  gdbwrap_errorhandler(desc, desc->packet);
	  return gdbwrap_run_length_decode(desc->packet, desc->packet,
					   desc->max_packet_size);
	}
      else
	{
	  DEBUGMSG(printf("received : %s, checksum: %#x, calulated: %#x\n",
			  desc->packet,
			  gdbwrap_atoh(checksum, strlen(checksum)),
			  gdbwrap_calc_checksum(desc, desc->packet)));
	  if (gdbwrap_is_interrupted(desc))
	    {
	      desc->interrupted = FALSE;
	      gdbwrap_errorhandler(desc, desc->packet);
	      return gdbwrap_run_length_decode(desc->packet, desc->packet,
					       desc->max_packet_size);
	    }
	  else
	    {
	      fprintf(stderr, "Muh ?\n");
	      return NULL;
	    }
	}
    }
  else desc->is_active = FALSE;

  return NULL;
}


static char          *gdbwrap_send_data(gdbwrap_t *desc, const char *query)
{
  int                 rval = 0;
  char               *mes;

  ASSERT(desc != NULL && query != NULL);

  DEBUGMSG(printf("Sending: %s\n", query));

  if (gdbwrap_is_active(desc))
    {
      do
	{
	  mes  = gdbwrap_make_message(desc, query);
	  rval = send(desc->fd, mes, strlen(mes), 0);
	} while(gdbwrap_check_ack(desc) != TRUE);

      ASSERT(rval != -1);
      mes  = gdbwrap_get_packet(desc);
      DEBUGMSG(printf("Received: %s\n", mes));
    }
  else
    {
      gdbwrap_errorhandler(desc, GDBWRAP_DEAD);
      mes = NULL;
    }

  return mes;
}


/******************** External functions ********************/


/**
 * Returns the last signal. We return the signal number or 0 if no
 * signal was returned.
 **/
unsigned             gdbwrap_lastsignal(gdbwrap_t *desc)
{
  unsigned           ret = 0;
  char               *lastmsg = gdbwrap_lastmsg(desc);

  /* When we receive a packet starting with GDBWRAP_SIGNAL_RECV, the
     next 2 characters reprensent the signal number. */
  if (lastmsg != NULL && (lastmsg[0] == GDBWRAP_SIGNAL_RECV ||
			  lastmsg[0] == GDBWRAP_SIGNAL_RECV2))
    ret = gdbwrap_atoh(lastmsg + 1, BYTE_IN_CHAR * sizeof(char));

  return ret;
}


u_char               gdbwrap_lasterror(gdbwrap_t *desc)
{
  u_char             ret = 0;
  char               *lastmsg = gdbwrap_lastmsg(desc);

  /* When we receive a packet starting with GDBWRAP_SIGNAL_RECV, the
     next 2 characters reprensent the signal number. */
  if (lastmsg != NULL && lastmsg[0] == GDBWRAP_REPLAY_ERROR)
    ret = gdbwrap_atoh(lastmsg + 1, BYTE_IN_CHAR * sizeof(char));

  return ret;
}


Bool                 gdbwrap_is_active(gdbwrap_t *desc)
{
  if (desc->is_active)
    return TRUE;
  else
    return FALSE;
}


/* If the last command is not supported, we return TRUE. */
Bool                 gdbwrap_cmdnotsup(gdbwrap_t *desc)
{
  char               *lastmsg = gdbwrap_lastmsg(desc);

  if (lastmsg[0] == GDBWRAP_NULL_CHAR)
    return TRUE;
  else
    return FALSE;
}


Bool                 gdbwrap_erroroccured(gdbwrap_t *desc)
{
  return desc->erroroccured;
}


unsigned             gdbwrap_atoh(const char *str, unsigned size)
{
  unsigned           i;
  unsigned           hex;

  for (i = 0, hex = 0; i < size; i++)
    if (str != NULL && str[i] >= 'A' && str[i] <= 'F')
      hex += (str[i] - 0x37) << 4 * (size - i - 1);
    else if (str != NULL && str[i] >= 'a' && str[i] <= 'f')
      hex += (str[i] - 0x57) << 4 * (size - i - 1);
    else if (str != NULL && str[i] >= '0' && str[i] <= '9')
      hex += (str[i] - 0x30) << 4 * (size - i - 1);
    else
      return 0;
    
  return hex;
}




/**
 * Set/Get the gdbwrapworld variable. It's not mandatory to use the
 * other functions, but sometimes a global variable is required.
 */
gdbwrapworld_t       gdbwrap_current_set(gdbwrap_t *world)
{
  gdbwrapworld.gdbwrapptr = world;

  return gdbwrapworld;
}


gdbwrap_t            *gdbwrap_current_get(void)
{
  return gdbwrapworld.gdbwrapptr;
}


/**
 * Initialize the descriptor. We provide a default value of 1000B for
 * the string that get the replies from server.
 *
 */
gdbwrap_t            *gdbwrap_init(int fd)
{
  gdbwrap_t          *desc = malloc(sizeof(gdbwrap_t));

  ASSERT(fd && desc != NULL);
  desc->max_packet_size   = 2500;
  desc->packet            = malloc((desc->max_packet_size + 1) * sizeof(char));
  desc->fd                = fd;
  desc->is_active         = TRUE;
  desc->interrupted       = FALSE;
  ASSERT(desc->packet != NULL);

  return desc;
}


void                gdbwrap_close(gdbwrap_t *desc)
{
  ASSERT(desc != NULL && desc->packet != NULL);
  free(desc->packet);
  free(desc);
}


/**
 * Initialize a connection with the gdb server and allocate more
 * memory for packets if necessary.
 *
 */
void                gdbwrap_hello(gdbwrap_t *desc)
{
  char              *received    = NULL;
  char              *result      = NULL;
  unsigned          previousmax  = 0;

  received = gdbwrap_send_data(desc, GDBWRAP_QSUPPORTED);

  if (received != NULL)
    {
      result   = gdbwrap_extract_from_packet(received, desc->packet,
					     GDBWRAP_PACKETSIZE,
					     GDBWRAP_SEP_SEMICOLON,
					     desc->max_packet_size);

      /* If we receive the info, we update gdbwrap_max_packet_size. */
      if (result != NULL)
	{
	  char *reallocptr;

	  previousmax = desc->max_packet_size;
	  desc->max_packet_size = gdbwrap_atoh(desc->packet, strlen(desc->packet));
	  reallocptr = realloc(desc->packet, desc->max_packet_size + 1);
	  if (reallocptr != NULL)
	    desc->packet = reallocptr;
	  else
	    {
	      gdbwrap_errorhandler(desc, GDBWRAP_NO_SPACE);
	      desc->max_packet_size = previousmax;
	    }
	}
      /* We set the last bit to a NULL char to avoid getting out of the
	 weeds with a (unlikely) bad strlen. */
      desc->packet[desc->max_packet_size] = GDBWRAP_NULL_CHAR;
    }
}


/**
 * Send a "disconnect" command to the server and free the packet.
 */
void                gdbwrap_bye(gdbwrap_t *desc)
{
  assert(desc != NULL);
  gdbwrap_send_data(desc, GDBWRAP_DISCONNECT);
  printf("\nThx for using gdbwrap :)\n");
}


void                gdbwrap_reason_halted(gdbwrap_t *desc)
{
  char              *received;

  received = gdbwrap_send_data(desc, GDBWRAP_WHY_HALTED);
  if (gdbwrap_is_active(desc))
    gdbwrap_populate_reg(desc, received);
  else
    gdbwrap_errorhandler(desc, GDBWRAP_NO_TABLE);
}


/**
 * Great, the gdb protocol has absolutely no consistency, thus we
 * cannot reuse the gdbwrap_populate_reg. We receive a poorly
 * documented bulk message when sending the "g" query.
 */
gdbwrap_gdbreg32     *gdbwrap_readgenreg(gdbwrap_t *desc)
{
  char               *rec;
  unsigned           i;
  ureg32             regvalue;

  rec = gdbwrap_send_data(desc, GDBWRAP_GENPURPREG);
  if (gdbwrap_is_active(desc))
    {
      for (i = 0; i < sizeof(gdbwrap_gdbreg32) / sizeof(ureg32); i++)
	{
	  /* 1B = 2 characters */
	  regvalue = gdbwrap_atoh(rec, 2 * DWORD_IN_BYTE);
	  regvalue = gdbwrap_little_endian(regvalue);
	  *(&desc->reg32.eax + i) = regvalue;
	  rec += 2 * DWORD_IN_BYTE;
	}

      return &desc->reg32;
    }
  else
    return NULL;
}


void                 gdbwrap_continue(gdbwrap_t *desc)
{
  char               *rec;

  if (gdbwrap_is_active(desc))
    {
      rec = gdbwrap_send_data(desc, GDBWRAP_CONTINUE);
      if (rec != NULL && gdbwrap_is_active(desc))
	gdbwrap_populate_reg(desc, rec);
    }
}


/**
 * Set a breakpoint. We read the value in memory, save it and write a
 * 0xcc in replacement. The usual command to set a bp is not supported
 * by the gdbserver.
 */
void                 gdbwrap_setbp(gdbwrap_t *desc, la32 linaddr, void *datasaved)
{
  u_char             bp = 0xcc;
  char               *ret;
  unsigned           atohresult;

  ASSERT(desc != NULL && desc != datasaved);
  ret = gdbwrap_readmem(desc, linaddr, 1);
  /* Fix: not clean. ATOH is not clean when returning an unsigned. */
  atohresult = gdbwrap_atoh(ret, 2 * 1);
  memcpy(datasaved, &atohresult, 1);
  gdbwrap_writemem(desc, linaddr, &bp, sizeof(u_char));
}


void                 gdbwrap_simplesetbp(gdbwrap_t *desc, la32 linaddr)
{
  char               packet[MSG_BUF];

  snprintf(packet, sizeof(packet), "%s%s%x%s%x", GDBWRAP_INSERTBP,
	   GDBWRAP_SEP_COMMA, linaddr, GDBWRAP_SEP_COMMA, 0x1);
  gdbwrap_send_data(desc, packet);
}


void                 gdbwrap_delbp(gdbwrap_t *desc, la32 linaddr, void *datasaved)
{
  gdbwrap_writemem(desc, linaddr, datasaved, sizeof(u_char));
}


void                 gdbwrap_simpledelbp(gdbwrap_t *desc, la32 linaddr)
{
  char               packet[MSG_BUF];

  snprintf(packet, sizeof(packet), "%s%s%x%s%x", GDBWRAP_REMOVEBP,
	   GDBWRAP_SEP_COMMA, linaddr, GDBWRAP_SEP_COMMA, 0x1);
  gdbwrap_send_data(desc, packet);
}


char                 *gdbwrap_readmem(gdbwrap_t *desc, la32 linaddr,
				      unsigned bytes)
{
  char               *rec;
  char               packet[MSG_BUF];

  snprintf(packet, sizeof(packet), "%s%x%s%x", GDBWRAP_MEMCONTENT,
	   linaddr, GDBWRAP_SEP_COMMA, bytes);
  rec = gdbwrap_send_data(desc, packet);

  return rec;
}


static void          *gdbwrap_writememory(gdbwrap_t *desc, la32 linaddr,
					  void *value, unsigned bytes)
{
  uint8_t            packetsize;
  char               *rec;
  char               *packet = alloca(bytes + MSG_BUF);

  ASSERT(desc != NULL && value != NULL);
  snprintf(packet, MSG_BUF, "%s%x%s%x%s", GDBWRAP_MEMWRITE,
	   linaddr, GDBWRAP_SEP_COMMA, bytes, GDBWRAP_SEP_COLON);
  packetsize = strlen(packet);
  ASSERT(packetsize < MSG_BUF);
  /* GDB protocol expects the value we send to be a "Binary value", ie
     not converted to a char. */
  memcpy(packet + packetsize, value, bytes);
  packet[packetsize + bytes] = GDBWRAP_NULL_CHAR;
  rec = gdbwrap_send_data(desc, packet);

  return rec;
}


static void          *gdbwrap_writememory2(gdbwrap_t *desc, la32 linaddr,
					   void *value, unsigned bytes)
{
  char               *rec;
  char               *packet = alloca(2 * bytes + MSG_BUF);
  u_char             *val = value;
  u_short            i;
  u_int              len;

  snprintf(packet, MSG_BUF, "%s%x%s%x%s", GDBWRAP_MEMWRITE2,
	   linaddr, GDBWRAP_SEP_COMMA, bytes, GDBWRAP_SEP_COLON);

  for (i = 0; i < bytes; i++)
    {
      len = strlen(packet);
      ASSERT(len + 1 < 2 * bytes + MSG_BUF);
      snprintf(packet + len, BYTE_IN_CHAR + 1, "%02x", (unsigned)val[i]);
    }
  rec = gdbwrap_send_data(desc, packet);

  return rec;
}


void                 gdbwrap_writemem(gdbwrap_t *desc, la32 linaddr,
				       void *value, unsigned bytes)
{
  static u_char      choice = 0;

  if (bytes)
    {
      do
	{
	  switch (choice)
	    {
	      case 0:
		gdbwrap_writememory(desc, linaddr, value, bytes);
		if (gdbwrap_cmdnotsup(desc))
		  choice++;
		break;

	      case 1:
		gdbwrap_writememory2(desc, linaddr, value, bytes);
		if (gdbwrap_cmdnotsup(desc))
		  choice++;
		break;

	      default:
		fprintf(stderr, "[W] Write to memory not supported.\n");
		break;
	    }
	} while (gdbwrap_cmdnotsup(desc) && choice < 2);
    }
}


/**
 * Write a specific register. This command seems not to be supported
 * by the gdbserver. See gdbwrap_writereg2.
 */
static void          gdbwrap_writeregister(gdbwrap_t *desc, ureg32 regNum,
					   la32 val)
{
  char               regpacket[MSG_BUF];

  ASSERT(desc != NULL);
  snprintf(regpacket, sizeof(regpacket), "%s%x=%x",
	   GDBWRAP_WRITEREG, regNum, val);
  gdbwrap_send_data(desc, regpacket);
}


static void          gdbwrap_writeregister2(gdbwrap_t *desc, ureg32 regNum,
					    la32 val)
{
  char               *ret;
  gdbwrap_gdbreg32   *reg;
  unsigned           offset;
  char               locreg[700];

  offset = 2 * regNum * sizeof(ureg32);

  ASSERT(desc != NULL && (regNum < sizeof(gdbwrap_gdbreg32) / sizeof(ureg32)) &&
	 offset + 2 * sizeof(ureg32) < desc->max_packet_size);
  reg = gdbwrap_readgenreg(desc);
  ret = gdbwrap_lastmsg(desc);
  ASSERT(reg != NULL && ret != NULL);

  snprintf(locreg, sizeof(locreg), "%08x", gdbwrap_little_endian(val));
  memcpy(ret + offset, locreg, 2 * sizeof(ureg32));
  snprintf(locreg, sizeof(locreg), "%s%s", GDBWRAP_WGENPURPREG, ret);
  gdbwrap_send_data(desc, locreg);
}


void                 gdbwrap_writereg(gdbwrap_t *desc, ureg32 regnum, la32 val)
{
  static u_char choice = 0;

  do
    {
      switch (choice)
	{
	  case 0:
	    gdbwrap_writeregister(desc, regnum, val);
	    if (gdbwrap_cmdnotsup(desc))
	      choice++;
	    break;

	  case 1:
	    gdbwrap_writeregister2(desc, regnum, val);
	    if (gdbwrap_cmdnotsup(desc))
	      choice++;
	    break;

	  default:
	    fprintf(stderr, "[W] Write to registers not supported.\n");
	    break;
	}
    } while (gdbwrap_cmdnotsup(desc) && choice < 2);

  if (choice < 2)
    *(&desc->reg32.eax + regnum) = val;
}


/**
 * Ship all the registers to the server in only 1 query. This is used
 * when modifying multiple registers at once for example.
 */
char                 *gdbwrap_shipallreg(gdbwrap_t *desc)
{
  gdbwrap_gdbreg32   savedregs;
  char               *ret;
  char               locreg[700];
  uint8_t            i;

  ASSERT(desc != NULL);
  memcpy(&savedregs, &desc->reg32, sizeof(gdbwrap_gdbreg32));

  gdbwrap_readgenreg(desc);
  ret = gdbwrap_lastmsg(desc);

  /* We modify the 9 GPR only and we copy the rest from the gpr
     request. */
  for (i = 0; i < 9; i++)
    snprintf(locreg + i * 2 * sizeof(ureg32), 2 * sizeof(ureg32) + 1,
	     "%08x", gdbwrap_little_endian(*(&savedregs.eax + i)));
  ASSERT(strlen(locreg) < desc->max_packet_size);
  memcpy(ret, locreg, strlen(locreg));
  snprintf(locreg, sizeof(locreg), "%s%s", GDBWRAP_WGENPURPREG, ret);

  return gdbwrap_send_data(desc, locreg);
}


void                gdbwrap_ctrl_c(gdbwrap_t *desc)
{
  u_char            sended = CTRL_C;
  int               rval;

  ASSERT(desc != NULL);
  desc->interrupted = TRUE;
  send(desc->fd, &sended, sizeof(u_char), 0);
  rval = recv(desc->fd, desc->packet, desc->max_packet_size, 0);
  gdbwrap_populate_reg(desc, desc->packet);
  rval = send(desc->fd, GDBWRAP_COR_CHECKSUM, strlen(GDBWRAP_COR_CHECKSUM),
	      0x0);
  ASSERT(rval);
}


/**
 * Here's the format of a signal:
 *
 * $vCont;C<signum>[:process_pid]#<checksum>
 *
 * Note that que process pid can be retrieved with a "X" command. If
 * process_pid is omited, then we apply to the current process
 * (default behavior).
 */
void                 gdbwrap_signal(gdbwrap_t *desc, int signal)
{
  char              *rec;
  char              signalpacket[MSG_BUF];

  ASSERT(desc != NULL);
  snprintf(signalpacket, sizeof(signalpacket), "%s;C%.2x",
	   GDBWRAP_CONTINUEWITH, signal);
  rec = gdbwrap_send_data(desc, signalpacket);
}


void                 gdbwrap_stepi(gdbwrap_t *desc)
{
  char               *rec;

  ASSERT(desc != NULL);
  rec = gdbwrap_send_data(desc, GDBWRAP_STEPI);

  if (gdbwrap_is_active(desc))
    gdbwrap_populate_reg(desc, rec);
  else
    gdbwrap_errorhandler(desc, GDBWRAP_DEAD);
}


/**
 * Sends a custom remote command. This heavily depends on the
 * server. We "transform" the char into its corresponding ASCII code
 * (in char).
 * @param: cmd the command to send, in clear text.
 **/
char                *gdbwrap_remotecmd(gdbwrap_t *desc, char *cmd)
{
  char              signalpacket[MSG_BUF];
  char              cmdcpy[MSG_BUF];
  char              *ret;
  uint8_t           i;
  uint8_t           rval;

  ASSERT(desc != NULL && cmd != NULL);
  /* We jump 2 in 2 chars, since 1B = 2chars. */
  for (i = 0; i < sizeof(cmdcpy) && cmd[i] != GDBWRAP_NULL_CHAR; i++)
    snprintf(cmdcpy + BYTE_IN_CHAR * i, BYTE_IN_CHAR + sizeof(GDBWRAP_NULL_CHAR),
	     "%02x", cmd[i]);

  snprintf(signalpacket, sizeof(signalpacket), "%s%s%s",
	   GDBWRAP_RCMD, GDBWRAP_SEP_COMMA, cmdcpy);
  ret = gdbwrap_send_data(desc, signalpacket);
  /* If we have a new line, it meens the packet is not finished (to
     prove...), we listen to the next incoming packet, which is an
     OK. */
  if (ret != NULL && gdbwrap_atoh(ret + strlen(ret) - 2, BYTE_IN_CHAR) == 0xa)
    {
      gdbwrap_send_ack(desc);
      rval = recv(desc->fd, cmdcpy, sizeof(cmdcpy), 0);
    }

  return ret;
}

/**
 * Get a memory map from the gdb server and
 * and return the information parsed on a gdbmemap_t.
 *
 */
gdbmemap_t          gdbwrap_memorymap_get(gdbwrap_t *desc)
{
  char              qXfer_msg[30]; //msg size
  gdbmemap_t        result;
  char              *received    = NULL;

  snprintf(qXfer_msg, sizeof(qXfer_msg), "%s::%d,%d",
	    GDBWRAP_MEMORYMAP_READ, 0, 0xfff);

  received = gdbwrap_send_data(desc, qXfer_msg);

  if (received != NULL)
    {
      //XXX: parse it and return gdbmemap_t
    }

  return result;
}
