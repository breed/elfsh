
class C_A {
private:
  int a_;

public:
  C_A() { a_ = 0; };

  void A_M_a () { a_++; };
};

class C_B {
 private:
  int b_;

  public:
  C_B() { b_ = 0; };
  void B_M_a () { b_++; };
};

class C_C : public C_A, public C_B 
{
 public:
  void C_M_a() { A_M_a(); B_M_a(); };
};


int main()
{
  C_C val;

  val.C_M_a();

  return 1;
}
