extsel.hpp 1: class ExtSelectWindow : public Window
extsel.hpp 2: {
extsel.hpp 3: int start,
extsel.hpp 4: end,
extsel.hpp 5: previous;
extsel.hpp 6: KEY *hotKeys;
extsel.hpp 7: int  (*func)(ExtSelectWindow&,int n,int y=0,int maxheight=0);
extsel.hpp 8: void (*action)(int);
extsel.hpp 9: ATTR barAttr;
extsel.hpp 10: int numEntries;
extsel.hpp 11: int offset[100];
extsel.hpp 12: 
extsel.hpp 13: void recalc(int order = 1);
extsel.hpp 14: void redraw(int = -1);
extsel.hpp 15: void scrollUp(int n);
extsel.hpp 16: void scrollDown(int n);
extsel.hpp 17: 
extsel.hpp 18: void keyDown();
extsel.hpp 19: void keyUp();
extsel.hpp 20: void keyPgUp();
extsel.hpp 21: void keyPgDn();
extsel.hpp 22: void keyHome();
extsel.hpp 23: void keyEnd();
extsel.hpp 24: 
extsel.hpp 25: public:
extsel.hpp 26: int current;
extsel.hpp 27: KEY hotkey;
extsel.hpp 28: 
extsel.hpp 29: ExtSelectWindow () { }
extsel.hpp 30: ExtSelectWindow (int num,ATTR battr,int (*f)(ExtSelectWindow&,int,int,int),KEY *hot=NULL,void(*actionf)(int)=NULLFUNC);
extsel.hpp 31: 
extsel.hpp 32: void define     (int num,ATTR battr,int (*f)(ExtSelectWindow&,int,int,int),KEY *hot=NULL,void(*actionf)(int)=NULLFUNC);
extsel.hpp 33: void reDefine   (int num);
extsel.hpp 34: 
extsel.hpp 35: int  process();
extsel.hpp 36: 
extsel.hpp 37: void show(int n);
extsel.hpp 38: void showLast()  { show(numEntries-1); }
extsel.hpp 39: void showFirst() { show(0); }
extsel.hpp 40: };
extsel.hpp 41: 
extsel.hpp 42: 