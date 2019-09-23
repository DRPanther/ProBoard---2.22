#include "proboard.hpp"

void
time_stat(char *)
{
LOG(2,"Time statistics requested");

io << "\n\f\n" << S_TIME_STATS_TITLE << '\n';

adjust_limits();

io << '\n' << S_STATS_TIME_ONLINE     (form("%d",timer.online()));
io << '\n' << S_STATS_TIME_LEFT       (form("%d",timer.left()));
io << '\n' << S_STATS_FREE_TIME       (form("%d",timer.timesuspended()));
io << '\n' << S_STATS_TIME_TODAY      (form("%ld",user.timeUsed));
io << '\n' << S_STATS_DAILY_TIME_LIMIT(form("%d",time_limit));
io << "\n\n" << S_STATS_DAILY_DL_LIMIT  (form("%d",download_limit));
io << '\n' << S_STATS_DOWNLOAD_LEFT   (form("%ld",long(download_limit)-user.kbToday));
io << '\n' << S_STATS_KB_DOWNLOAD     (form("%ld",user.kbDownloaded));
io << '\n' << S_STATS_KB_UPLOADED     (form("%ld",user.kbUploaded));
io << '\n' << S_STATS_KB_UPLOAD_NEEDED(form("%u",upload_needed));
io << '\n' << S_STATS_FREE_DOWNLOAD   (form("%d",free_download));
io << "\n\n" << S_STATS_LAST_CALLED   ((char *)user.lastDate.format("DD MMM CCYY"),(char *)user.lastTime.format("HH:MM:SS"));
io << '\n' << S_STATS_TOTAL_CALLS     (form("%ld",user.timesCalled));

io << "\n\n" << S_PRESS_ENTER_TO_CONTINUE;
}

