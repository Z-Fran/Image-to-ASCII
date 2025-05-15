/******************************************************************
* Note!                                                           *
* This header file is a low-level function package for WinAPI     *
* and WIC to handle image reading operations.                     *
* This package is external, so you need to follow these rules:    *
*     Do not modify any part of this header file without TO-DO,   *
*     if you have any questions, please contact the teacher.      *
*     Each TO-DO section is marked with TO-DO and TO-DO END.      *
*     The readPic() function is a low-level WinAPI method that    *
*     reads images as RGBA bitmap data. This cannot be modified   *
*     directly to achieve grayscale image reading.                *
*     The getData() function is a function that needs to be       *
*     implemented to convert the one-dimensional BYTE array into  *
*     the Array class you implemented.                            *
*     The testReader() function is a demo that provides a         *
*     reference for reading data.                                 *
******************************************************************/
#ifndef PIC_READER_H
#define PIC_READER_H

#include <windows.h>
#include <wincodec.h>
#include <commdlg.h>
#include "Array.h"
template <typename T>
inline void SafeRelease(T *&p) {
	if (nullptr != p) {
		p->Release();
		p = nullptr;
	}
}

class PicReader {
public:
	PicReader();
	~PicReader();

	void readPic(LPCSTR);
	Array /*TO-DO: You need to modify the return type END*/ getData(/*TO-DO: You need to modify the parameters END*/);
	void testReader(BYTE *&,UINT &, UINT &);
private:
	void init();
	bool checkHR(HRESULT);
	void quitWithError(LPCSTR);

	HWND                    hWnd;
	HANDLE                  hFile;
	IWICImagingFactory     *m_pIWICFactory;
	IWICFormatConverter    *m_pConvertedSourceBitmap;

	/*TO-DO: You may need to add internal members END*/
};

PicReader::PicReader() : m_pConvertedSourceBitmap(nullptr), m_pIWICFactory(nullptr) {
	init();
}

PicReader::~PicReader() {
	if (hFile != NULL) CloseHandle(hFile);
	SafeRelease(m_pConvertedSourceBitmap);
	SafeRelease(m_pIWICFactory);
	CoUninitialize();
}

bool PicReader::checkHR(HRESULT hr) {
	return (hr < 0);
}

void PicReader::quitWithError(LPCSTR message) {
	MessageBoxA(hWnd, message, "Application Error", MB_ICONEXCLAMATION | MB_OK);
	quick_exit(0xffffffff);
}

void PicReader::init() {
	hWnd = GetForegroundWindow();

	// Enables the terminate-on-corruption feature.
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

	HRESULT hr = S_OK;

	//Init the WIC
	hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// Create WIC factory
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pIWICFactory)
	);

	// Throw error if create factor failed
	if (checkHR(hr)) { quitWithError("Init Reader Failed"); }
}

void PicReader::readPic(LPCSTR fileName) {
	HRESULT hr = S_OK;

	// Create a File Handle (WinAPI method not std c)
	if (hFile != NULL) CloseHandle(hFile);
	hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (GetLastError() == ERROR_FILE_NOT_FOUND) {
		quitWithError("Cannot find such file, please retry or check the access");
	}

	// Create a decoder
	IWICBitmapDecoder *pDecoder = nullptr;
	hr = m_pIWICFactory->CreateDecoderFromFileHandle((ULONG_PTR)hFile, nullptr, WICDecodeMetadataCacheOnDemand, &pDecoder);
	if (checkHR(hr)) { quitWithError("Create Decoder Failed"); }

	// Retrieve the first frame of the image from the decoder
	IWICBitmapFrameDecode *pFrame = nullptr;
	hr = pDecoder->GetFrame(0, &pFrame);
	if (checkHR(hr)) { quitWithError("Get Frame Failed"); }

	// Format convert the frame to 32bppRGBA
	SafeRelease(m_pConvertedSourceBitmap);
	hr = m_pIWICFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
	if (checkHR(hr)) { quitWithError("Get Format Converter Failed"); }

	hr = m_pConvertedSourceBitmap->Initialize(pFrame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeCustom);
	if (checkHR(hr)) { quitWithError("Init Bitmap Failed"); }

	// Clean memory
	SafeRelease(pDecoder);
	SafeRelease(pFrame);
}

Array /*TO-DO: You need to modify the return type END*/ PicReader::getData(/*TO-DO: You need to modify the parameters END*/) {
	HRESULT hr = S_OK;

	// Get the size of Image
	UINT x, y;
	hr = m_pConvertedSourceBitmap->GetSize(&x, &y);
	if (checkHR(hr)) { quitWithError("Check Bitmap Size Failed"); }

	// Create the buffer of pixels, the type of BYTE is unsigned char
	BYTE *data;
	data = new BYTE[x * y * 4];
	memset(data, 0, x * y * 4);

	// Copy the pixels to the buffer
	UINT stride = x * 4;
	hr = m_pConvertedSourceBitmap->CopyPixels(nullptr, stride, x * y * 4, data);
	if (checkHR(hr)) { quitWithError("Copy Pixels Failed"); }

	/******************************************************************
	*  TO-DO:                                                         *
	*                                                                 *
	*  Implement an Array class to convert the data into your Array   *
	*                                                                 *
	*  Note: The Bitmap Copy data is stored in a one-dimensional      *
	*        array of size (width*height*4), with stride of 4 bytes   *
	*        per pixel in the order R G B A R G B A R G B A.....      *
	*                                                                 *
	*  Note!  You can only modify the code from here to the next      *
	*         TO-DO END position!                                     *
	******************************************************************/

	Array RGB_Data(y, x, (size_t)4);
	memcpy(RGB_Data.data, data, x * y * 4);
	delete[] data;

	/******************************************************************
	*  TO-DO END                                                      *
	******************************************************************/

	// Close the file handle
	CloseHandle(hFile);
	hFile = NULL;
	return RGB_Data;
}

void PicReader::testReader(BYTE* &_out, UINT& _x, UINT& _y){
	HRESULT hr = S_OK;

	// Get the size of Image
	UINT x, y;
	hr = m_pConvertedSourceBitmap->GetSize(&x, &y);
	if (checkHR(hr)) { quitWithError("Check Bitmap Size Failed"); }

	// Create the buffer of pixels, the type of BYTE is unsigned char
	BYTE *data;
	data = new BYTE[x * y * 4];
	memset(data, 0, x * y * 4);

	// Copy the pixels to the buffer
	UINT stride = x * 4;
	hr = m_pConvertedSourceBitmap->CopyPixels(nullptr, stride, x * y * 4, data);
	if (checkHR(hr)) { quitWithError("Copy Pixels Failed"); }


	_out = data; _x = x; _y = y;

	// Close the file handle
	CloseHandle(hFile);
	hFile = NULL;
}

#endif