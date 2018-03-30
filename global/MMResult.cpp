#include "..\\global\\MMResult.h"

Result Error(MMRESULT mmr)
{
	if(mmr == MMSYSERR_NOERROR)
		return R_OK;
	else
	{
		TCHAR err[MAXERRORLENGTH];
		waveOutGetErrorText(mmr, err, MAXERRORLENGTH);
		return Result(String("WinMM: ") << String(err));
	}
}
Result Error(MMRESULT mmr, LPCTSTR file, LPCTSTR func, int line)
{
	if(mmr == MMSYSERR_NOERROR)
		return R_OK;
	else
	{
		TCHAR err[MAXERRORLENGTH];
		waveOutGetErrorText(mmr, err, MAXERRORLENGTH);
		return Result(String("WinMM: ") << String(err), file, func, line);
	}
}