class ExtSelectWindow : public Window
   {
      int start,
          end,
          previous;
      KEY *hotKeys;
      int  (*func)(ExtSelectWindow&,int n,int y=0,int maxheight=0);
      void (*action)(int);
      ATTR barAttr;
      int numEntries;
      int offset[100];

      void recalc(int order = 1);
      void redraw(int = -1);
      void scrollUp(int n);
      void scrollDown(int n);

      void keyDown();
      void keyUp();
      void keyPgUp();
      void keyPgDn();
      void keyHome();
      void keyEnd();

   public:
      int current;
      KEY hotkey;

      ExtSelectWindow () { }
      ExtSelectWindow (int num,ATTR battr,int (*f)(ExtSelectWindow&,int,int,int),KEY *hot=NULL,void(*actionf)(int)=NULLFUNC);

      void define     (int num,ATTR battr,int (*f)(ExtSelectWindow&,int,int,int),KEY *hot=NULL,void(*actionf)(int)=NULLFUNC);
      void reDefine   (int num);

      int  process();

      void show(int n);
      void showLast()  { show(numEntries-1); }
      void showFirst() { show(0); }
   };
