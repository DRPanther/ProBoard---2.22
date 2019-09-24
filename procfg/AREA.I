area.hpp 1: class Area
area.hpp 2: {
area.hpp 3: static int selected_start;
area.hpp 4: static int selected_end;
area.hpp 5: public:
area.hpp 6: virtual ~Area() {}
area.hpp 7: 
area.hpp 8: virtual bool read(int)   = 0;
area.hpp 9: virtual bool write(int)  = 0;
area.hpp 10: virtual bool clear(int)  = 0;
area.hpp 11: 
area.hpp 12: virtual bool insert(int) = 0;
area.hpp 13: virtual bool remove(int) = 0;
area.hpp 14: 
area.hpp 15: virtual int  numAreas()  = 0;
area.hpp 16: 
area.hpp 17: virtual KEY edit()       = 0;
area.hpp 18: 
area.hpp 19: virtual int  selStart(int i = -20000)
area.hpp 20: {
area.hpp 21: if(i != -20000)
area.hpp 22: selected_start = i;
area.hpp 23: 
area.hpp 24: return selected_start;
area.hpp 25: }
area.hpp 26: virtual int  selEnd  (int i = -20000)
area.hpp 27: {
area.hpp 28: if(i != -20000)
area.hpp 29: selected_end = i;
area.hpp 30: 
area.hpp 31: return selected_end;
area.hpp 32: }
area.hpp 33: virtual void listFunc(int,int,int) = 0;
area.hpp 34: virtual dword crc() = 0;
area.hpp 35: 
area.hpp 36: virtual bool sort(int,int) = 0;
area.hpp 37: virtual bool move(int,int,int) = 0;
area.hpp 38: };
area.hpp 39: 
area.hpp 40: 
area.hpp 41: class MsgArea : public _MsgArea , public Area
area.hpp 42: {
area.hpp 43: static File f;
area.hpp 44: static DynamicArrayFile<_MsgArea> *df;
area.hpp 45: static int file_count;
area.hpp 46: 
area.hpp 47: static _MsgArea fEmpty(long);
area.hpp 48: static void     fMove(long,long,_MsgArea&);
area.hpp 49: public:
area.hpp 50: 
area.hpp 51: 
area.hpp 52: MsgArea();
area.hpp 53: ~MsgArea();
area.hpp 54: 
area.hpp 55: bool read(int);
area.hpp 56: bool write(int);
area.hpp 57: bool clear(int);
area.hpp 58: 
area.hpp 59: bool insert(int);
area.hpp 60: bool remove(int);
area.hpp 61: 
area.hpp 62: int  numAreas();
area.hpp 63: 
area.hpp 64: KEY edit();
area.hpp 65: 
area.hpp 66: void listFunc(int,int x,int y);
area.hpp 67: 
area.hpp 68: dword crc();
area.hpp 69: 
area.hpp 70: bool  sort(int,int);
area.hpp 71: bool  move(int,int,int);
area.hpp 72: };
area.hpp 73: 
area.hpp 74: class FileArea : public _FileArea , public Area
area.hpp 75: {
area.hpp 76: static File f;
area.hpp 77: static DynamicArrayFile<_FileArea> *df;
area.hpp 78: static int file_count;
area.hpp 79: 
area.hpp 80: static _FileArea fEmpty(long);
area.hpp 81: 
area.hpp 82: int areaNum;
area.hpp 83: 
area.hpp 84: public:
area.hpp 85: FileArea();
area.hpp 86: ~FileArea();
area.hpp 87: 
area.hpp 88: bool read(int);
area.hpp 89: bool write(int);
area.hpp 90: bool clear(int);
area.hpp 91: 
area.hpp 92: bool insert(int);
area.hpp 93: bool remove(int);
area.hpp 94: 
area.hpp 95: int  numAreas();
area.hpp 96: 
area.hpp 97: KEY  edit();
area.hpp 98: 
area.hpp 99: void listFunc(int,int x,int y);
area.hpp 100: 
area.hpp 101: dword crc();
area.hpp 102: 
area.hpp 103: bool  sort(int,int);
area.hpp 104: bool  move(int,int,int);
area.hpp 105: };
area.hpp 106: 
area.hpp 107: 