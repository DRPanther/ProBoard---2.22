#define Use_MsgBase

#include <string.h>
#include "proboard.hpp"

void
Message::remove()
{
 if(next || prev) delReply();

 if(!msgBase()->deleteMsg(*this)) LOG("Error deleting message");
}
