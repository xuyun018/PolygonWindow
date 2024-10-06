#include <Windows.h>

#include <stdint.h>
#include "gif/gif.h"

unsigned char *ReadFileBuffer(const WCHAR *filename, unsigned int *filesize)
{
	HANDLE hfile;
	DWORD numberofbytes;
	unsigned char *result = NULL;

	hfile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile != INVALID_HANDLE_VALUE)
	{
		*filesize = GetFileSize(hfile, NULL);

		result = (unsigned char *)malloc(*filesize);
		if (result != NULL)
		{
			ReadFile(hfile, result, *filesize, &numberofbytes, NULL);
		}

		CloseHandle(hfile);
	}
	return(result);
}

int wmain(int argc, WCHAR *argv[])
{
	struct gif_reader pgif[1];
	unsigned char *img;
	unsigned char *buffer;
	unsigned int buffersize;
	uint16_t width, height;

	if (argc > 1)
	{
		buffer = ReadFileBuffer(argv[1], &buffersize);
		if (buffer)
		{
			if (gifr_init(pgif, GIF_BUFFER, buffer) == 0)
			{
				width = pgif->width;
				height = pgif->height;

				while (1)
				{
					gifr_head(pgif);

					for (int t = 0; t < pgif->n_frames; t++)
					{
						if (gifr_next(pgif))
						{
							HDC hdc = GetDC(NULL);
							const unsigned char *p = pgif->image;
							for (int j = 0; j < height; j++)
							{
								for (int i = 0; i < width; i++)
								{
									SetPixel(hdc, i, j, RGB(p[0], p[1], p[2]));
									p += 3;
								}
							}
							ReleaseDC(NULL, hdc);
						}
					}
				}

				gifr_deinit(pgif);
			}

			free(buffer);
		}
	}

	Sleep(10000);

	return(0);
}