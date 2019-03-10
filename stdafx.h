
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifdef _DEBUG
// ���û�а�װVLD����Ҫע��������
#ifdef _M_IX86
#include "vld.h"
#endif
#endif

// �Ƴ��Ի�����MFC�ؼ���֧�֣���С��̬�������Ĵ�С
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#define USING_GLOG 0

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// ����Ϊ�������

// ���� SIP request ͷ(CseqΪ����)
#define SIP_RequestHeader_i(pFrom, pTo, pCallId, pSeq, nXmlLen) \
	"INVITE sip:%s SIP/2.0\r\n"\
	"Via: SIP/2.0/TCP %s\r\n"\
	"To: %s\r\n"\
	"From: %s\r\n"\
	"Max-Forwards: 70\r\n"\
	"Call-ID: %s\r\n"\
	"CSeq: %d INVITE\r\n"\
	"Content-Type: RVSS/xml\r\n"\
	"Content-Length: %d\r\n\r\n", pTo, pFrom, pTo, pFrom, pCallId, pSeq, nXmlLen

// �汾��Ϣ
#define KEEPER_VERSION "KeepApplication-Ver1.0"

// ����ʱ��
#define KEEPER_DATETIME 201804

// ��ȡע�������
void GetRegisterPkg(char *reg, const char *from, const char *to);
