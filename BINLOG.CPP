#include "proboard.hpp"
#include <string.h>

const int BINLOG_BUF = 10;

bool
BinLog::append()
{
    File f(FN_BINLOG_PB,fmode_rw | fmode_copen | fmode_denywr);

    if(!f.opened()) return FALSE;

    if(f.len() >= sizeof(BinLog))
    {
        Date today(TODAY);
        BinLog *log = new BinLog[ BINLOG_BUF ];

        f.seek(0);

        f.read(log,sizeof(BinLog));

        if(cfg.binlogdays && (today - log->date) > cfg.binlogdays)
        {
            f.seek(0);
            for(long rec = 0;;rec++)
            {
                if(f.read(log,sizeof(BinLog)) != sizeof(BinLog)) break;

                if((today - log->date) <= cfg.binlogdays)
                {
                    long dest_rec = 0;

                    for(;;)
                    {
                        f.seek(rec * sizeof(BinLog));

                        word n = f.read(log,sizeof(BinLog) * BINLOG_BUF);

                        if(n < sizeof(BinLog)) break;

                        n /= sizeof(BinLog);

                        f.seek(dest_rec * sizeof(BinLog));
                        f.write(log,n * sizeof(BinLog));

                        dest_rec += n;
                        rec      += n;
                    }

                    f.seek(dest_rec * sizeof(BinLog));
                    f.cut();

                    break;
                }
            }
        }

        delete [] log;
    }

    f.seek(f.len() / sizeof(BinLog) * sizeof(BinLog));

    f.write(this,sizeof(*this));

    return TRUE;
}

BinLog::BinLog()
{
    memset(this,0,sizeof(*this));
}
