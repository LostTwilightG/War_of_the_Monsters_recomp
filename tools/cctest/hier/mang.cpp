class A {
public:
    static int s1(void);
    static int s4(void) { return 4; }
    int n4(void) { return 5; }
    static int d;
    static int s5();
    int n5();
};
int A::d = 3;
int A::s1(void) { return d; }
int A::s5() { return 6; }
int A::n5() { return 7; }
int f(A *a) { return A::s4() + a->n4() + A::s5() + a->n5() + A::s1(); }
