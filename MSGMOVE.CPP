#define Use_MsgBase

#include <stdlib.h>
#include "proboard.hpp"

void
Message::move()
{
    MsgArea ma;
    int areanum = 0;

    io << "\n\n";
    linecounter(0);
    set_msgarea(form("* -N -%d /L" , msgArea->areaNum));
    io << '\n' << S_SELECT_MSG_AREA_TO_MOVE_MESSAGE_TO;
    io.read(areanum,5);

    if(areanum < 1) return;

    if(!ma.read(areanum))
      {
       io << "\n\n" << S_UNKNOWN_MESSAGE_AREA << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
       return;
      }

    io << "\n\n" << S_MOVING_MESSAGE;

    Message msg(this,areanum);

    createMsgTextFile("MSGTMP",FALSE);

    remove();

    msg.add();

    io << "\n\n";
}
