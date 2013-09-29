// nyxpng.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


static const _TCHAR* progname;
static const _TCHAR* outdir;

void usage()
{
	_tprintf(_T("Usage: %s <outdir> <srcfile> [srcfile ...]\n"), progname);
}

int _tmain(int argc, _TCHAR* argv[])
{
	progname = *argv;
	argc--;
	argv++;

	if(argc <= 0)
	{
		_tprintf(_T("Error: Output dir not specified!\n"));
		usage();
		return 1;
	}
	outdir = *argv;
	argc--;
	argv++;

	if(argc <= 0)
	{
		_tprintf(_T("Error: Source file not specified!\n"));
		usage();
		return 1;
	}

	while(argc > 0)
	{
		unsigned int size;
		int err;
		unsigned char *uncrush;

#ifdef _UNICODE
		size_t r;
		char path[256];
		const _TCHAR *ipath = *argv;
		mbstate_t st = {0};

		wcsrtombs_s(&r, path, _countof(path), &ipath, _countof(path), &st);
#else
		const char *path = *argv;
#endif

		uncrush = npt_create_uncrushed_from_file(path, &size, &err);
		if(size == 0)
		{
#ifdef _UNICODE
			_TCHAR msg[256];
			const char *ierrmsg = npt_error_message(err);
			mbsrtowcs_s(&r, msg, _countof(msg), &ierrmsg, _countof(msg), &st);
#else
			const char *msg = npt_error_message(err);
#endif

			_tprintf(_T("Warning: %s: uncrush failed: %s\n"), *argv, msg);
		} else
		{
			_TCHAR name[256];
			_TCHAR ext[256];
			_TCHAR nameext[256];
			_tsplitpath_s(*argv, NULL, 0, NULL, 0, name, _countof(name), ext, _countof(ext));
			_stprintf_s(nameext, _countof(nameext), _T("%s\\%s%s"), outdir, name, ext); // ext contains "."
			_tprintf(_T("%s\n"), nameext);

#ifdef _UNICODE
			char nameext_a[256];
			const _TCHAR *inameext = nameext;
			wcsrtombs_s(&r, nameext_a, _countof(nameext_a), &inameext, _countof(nameext_a), &st);
#else
			const char *nameext_a = nameext;
#endif

			FILE *fp;
			if(fopen_s(&fp, nameext_a, "wb") < 0)
			{
				_tprintf(_T("Warning: %s: could not open!\n"), nameext);
			} else
			{
				fwrite(uncrush, size, 1, fp);
				fclose(fp);
			}
			free(uncrush);
		}

		argc--;
		argv++;
	}

	return 0;
}

