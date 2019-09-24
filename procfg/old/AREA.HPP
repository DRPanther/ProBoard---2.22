class Area
   {
      static int selected_start;
      static int selected_end;
   public:
      virtual ~Area() {}

      virtual bool read(int)   = 0;
      virtual bool write(int)  = 0;
      virtual bool clear(int)  = 0;

      virtual bool insert(int) = 0;
      virtual bool remove(int) = 0;

      virtual int  numAreas()  = 0;

      virtual KEY edit()       = 0;

      virtual int  selStart(int i = -20000)
         {
            if(i != -20000)
               selected_start = i;

            return selected_start;
         }
      virtual int  selEnd  (int i = -20000)
         {
            if(i != -20000)
               selected_end = i;

            return selected_end;
         }
      virtual void listFunc(int,int,int) = 0;
      virtual dword crc() = 0;

      virtual bool sort(int,int) = 0;
      virtual bool move(int,int,int) = 0;
   };


class MsgArea : public _MsgArea , public Area
   {
      static File f;
      static DynamicArrayFile<_MsgArea> *df;
      static int file_count;

      static _MsgArea fEmpty(long);
      static void     fMove(long,long,_MsgArea&);
   public:


      MsgArea();
      ~MsgArea();

      bool read(int);
      bool write(int);
      bool clear(int);

      bool insert(int);
      bool remove(int);

      int  numAreas();

      KEY edit();

      void listFunc(int,int x,int y);

      dword crc();

      bool  sort(int,int);
      bool  move(int,int,int);
   };

class FileArea : public _FileArea , public Area
   {
      static File f;
      static DynamicArrayFile<_FileArea> *df;
      static int file_count;

      static _FileArea fEmpty(long);

      int areaNum;

   public:
      FileArea();
      ~FileArea();

      bool read(int);
      bool write(int);
      bool clear(int);

      bool insert(int);
      bool remove(int);

      int  numAreas();

      KEY  edit();

      void listFunc(int,int x,int y);

      dword crc();

      bool  sort(int,int);
      bool  move(int,int,int);
   };
