/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013 Jozef Pridavok
 Copyright (c) 2017 zilongshanren

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "UIEditBoxImpl-win32.h"

#include "platform/CCPlatformConfig.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

#include "UIEditBox.h"
#include <tchar.h>
#include <stdio.h>
#include "2d/CCLabel.h"
#include "base/CCDirector.h"
#include "base/ccUTF8.h"
#include <Commctrl.h>
#include <windows.h>

NS_CC_BEGIN

namespace ui {

    bool EditBoxImplWin::s_isInitialized = false;
    int EditBoxImplWin::s_editboxChildID = 100;
    HWND EditBoxImplWin::s_previousFocusWnd = 0;
    WNDPROC EditBoxImplWin::s_prevCocosWndProc = 0;
    HINSTANCE EditBoxImplWin::s_hInstance = 0;
    HWND EditBoxImplWin::s_hwndCocos = 0;
    void EditBoxImplWin::lazyInit()
    {
        s_hwndCocos = cocos2d::Director::getInstance()->getOpenGLView()->getWin32Window();
        LONG style = GetWindowLong(s_hwndCocos, GWL_STYLE);
        SetWindowLong(s_hwndCocos, GWL_STYLE, style | WS_CLIPCHILDREN);
        s_isInitialized = true;
        s_previousFocusWnd = s_hwndCocos;

        s_hInstance = GetModuleHandle(NULL);


        s_prevCocosWndProc = (WNDPROC)SetWindowLongPtr(s_hwndCocos, GWL_WNDPROC, (LONG_PTR)hookGLFWWindowProc);

    }

    EditBoxImpl* __createSystemEditBox(EditBox* pEditBox)
    {
        return new (std::nothrow) EditBoxImplWin(pEditBox);
    }

    EditBoxImplWin::EditBoxImplWin(EditBox* pEditText)
        : EditBoxImplCommon(pEditText)
    {
        if (!s_isInitialized)
        {
            lazyInit();
        }
        
        s_editboxChildID++;

    }

    EditBoxImplWin::~EditBoxImplWin()
    {
        if (hwndEdit)
        {
            SetWindowLongPtr(hwndEdit, GWL_WNDPROC, (LONG_PTR)_prevWndProc);
            DestroyWindow(hwndEdit);
        }
    }


    bool EditBoxImplWin::isEditing()
    {
        return false;
    }

    void EditBoxImplWin::createNativeControl(const Rect & frame)
    {
        hwndEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE, L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_CHILD | ES_LEFT | WS_BORDER | WS_EX_TRANSPARENT | WS_TABSTOP,
            frame.origin.x,
            frame.origin.y,
            frame.size.width,
            frame.size.height,   // set size in WM_SIZE message 
            s_hwndCocos,         // parent window 
            (HMENU)s_editboxChildID,   // edit control ID 
            s_hInstance,
            this);        // pointer not needed 

        // Clear the password style
        SendMessage(hwndEdit, EM_SETPASSWORDCHAR, (WPARAM)0, (LPARAM)0);

        //register new window proc func
        SetWindowLongPtr(hwndEdit, GWL_USERDATA, (LONG_PTR)this);
        _prevWndProc = (WNDPROC)SetWindowLongPtr(hwndEdit, GWL_WNDPROC, (LONG_PTR)WindowProc);


    }

    void EditBoxImplWin::setNativeFont(const char * pFontName, int fontSize)
    {
    }



    void EditBoxImplWin::setNativeFontColor(const Color4B & color)
    {
    }

    void EditBoxImplWin::setNativePlaceholderFont(const char * pFontName, int fontSize)
    {
    }

    void EditBoxImplWin::setNativePlaceholderFontColor(const Color4B& color)
    {

    }

    void EditBoxImplWin::setNativeInputMode(EditBox::InputMode inputMode)
    {
        // Allows only digits to be entered into the edit control.
        // Note that, even with this set, it is still possible to paste non-digits into the edit control.
        // Please refer to https://msdn.microsoft.com/en-us/library/bb775464%28v=vs.85%29.aspx
        // ::SetWindowLongW(hwndEdit1, GWL_STYLE, ::GetWindowLongW(hwndEdit1, GWL_STYLE) | ES_NUMBER);
        // ::SetWindowLongW(hwndEdit2, GWL_STYLE, ::GetWindowLongW(hwndEdit2, GWL_STYLE) | ES_NUMBER);
    }
    void EditBoxImplWin::setNativeInputFlag(EditBox::InputFlag inputFlag)
    {
        if (inputFlag == EditBox::InputFlag::PASSWORD)
        {
            //SendMessage(hwndEdit, EM_SETPASSWORDCHAR, (WPARAM)'*', (LPARAM)0);

        }
        else if (inputFlag == EditBox::InputFlag::INITIAL_CAPS_ALL_CHARACTERS)
        {

        }

        // // Converts all characters to lowercase as they are typed into the edit control.
        // // Please refer to https://msdn.microsoft.com/en-us/library/bb775464%28v=vs.85%29.aspx
        // ::SetWindowLongW(hwndEdit1, GWL_STYLE, ::GetWindowLongW(hwndEdit1, GWL_STYLE) | ES_UPPERCASE);
        // ::SetWindowLongW(hwndEdit2, GWL_STYLE, ::GetWindowLongW(hwndEdit2, GWL_STYLE) | ES_UPPERCASE);
    }
    void EditBoxImplWin::setNativeReturnType(EditBox::KeyboardReturnType returnType)
    {

    }
    void EditBoxImplWin::setNativeText(const char* pText)
    {

    }
    void EditBoxImplWin::setNativePlaceHolder(const char* pText)
    {

    }
    void EditBoxImplWin::setNativeVisible(bool visible)
    {
        if (visible)
        {
            ::ShowWindow(hwndEdit, SW_SHOW);
        }
        else
        {
            ::ShowWindow(hwndEdit, SW_HIDE);
        }
    }
    void EditBoxImplWin::updateNativeFrame(const Rect& rect)

    {
        ::SetWindowPos(
            hwndEdit,
            HWND_NOTOPMOST,
            rect.origin.x,
            rect.origin.y,
            rect.size.width,
            rect.size.height,
            SWP_NOZORDER);

    }
    const char* EditBoxImplWin::getNativeDefaultFontName()
    {
        return "Arial";
    }
    void EditBoxImplWin::nativeOpenKeyboard()
    {
        if (s_previousFocusWnd != s_hwndCocos) {
            ::ShowWindow(s_previousFocusWnd, SW_HIDE);
        }

        ::PostMessage(hwndEdit, WM_SETFOCUS, (WPARAM)s_previousFocusWnd, 0);
        s_previousFocusWnd = hwndEdit;
        this->editBoxEditingDidBegin();
       
    }
    void EditBoxImplWin::nativeCloseKeyboard()
    {
       // ::PostMessage(hwndEdit, WM_KILLFOCUS, 0, 0);
    }
    void EditBoxImplWin::setNativeMaxLength(int maxLength)
    {
        // // Sets the text limit of the edit control.
        // // wParam is the maximum number of WCHARs the user can enter. For Unicode text, wParam is the number of characters.
        // // Please refer to https://msdn.microsoft.com/en-us/library/bb761607%28v=vs.85%29.aspx
        // if ((int)_param->nMaxLength > 0)
        // {
        //     ::SendMessageW(hwndEdit1, EM_LIMITTEXT, (WPARAM)_param->nMaxLength, 0);
        //     ::SendMessageW(hwndEdit2, EM_LIMITTEXT, (WPARAM)_param->nMaxLength, 0);
        // }
    }


    void EditBoxImplWin::_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CHAR:
            if (wParam == VK_RETURN)
            {
                this->editBoxEditingReturn();
                this->editBoxEditingDidEnd(this->getText());
            }
            break;
        case WM_SETFOCUS:
            ::PostMessage(hwnd, WM_ACTIVATE, (WPARAM)s_previousFocusWnd, 0);
            ::PostMessage(hwnd, WM_SETCURSOR, (WPARAM)s_previousFocusWnd, 0);
            s_previousFocusWnd = hwndEdit;
            break;
        case WM_KILLFOCUS:
            //when app enter background, this message also be called.
            if (this->_editingMode && !IsWindowVisible(hwnd))
            {
                this->editBoxEditingReturn();
                this->editBoxEditingDidEnd(this->getText());
            }
            break;
        default:
        break;

        }
    }

    std::string EditBoxImplWin::getText()const
    {
        std::u16string wstrResult;
        std::string utf8Result;


        int inputLength = ::GetWindowTextLengthW(this->hwndEdit);
        wstrResult.resize(inputLength);

        ::GetWindowTextW(this->hwndEdit, (LPWSTR) const_cast<char16_t*>(wstrResult.c_str()), inputLength + 1);
        bool conversionResult = cocos2d::StringUtils::UTF16ToUTF8(wstrResult, utf8Result);
        if (!conversionResult)
        {
            CCLOG("warning, editbox input text convertion error.");
        }
        return std::move(utf8Result);
    }

    LRESULT EditBoxImplWin::hookGLFWWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_COMMAND:
            if (HIWORD(wParam) == EN_CHANGE) {
                EditBoxImplWin* pThis = (EditBoxImplWin*)GetWindowLongPtr((HWND)lParam, GWLP_USERDATA);
                if (pThis)
                {
                    pThis->editBoxEditingChanged(pThis->getText());
                }

            }
            break;
        case WM_LBUTTONDOWN:
            if (s_previousFocusWnd != s_hwndCocos) {
                ::ShowWindow(s_previousFocusWnd, SW_HIDE);
                ::PostMessage(s_hwndCocos, WM_SETFOCUS, (WPARAM)s_previousFocusWnd, 0);
                ::PostMessage(s_hwndCocos, WM_ACTIVATE, (WPARAM)s_previousFocusWnd, 0);
                ::PostMessage(s_hwndCocos, WM_SETCURSOR, (WPARAM)s_previousFocusWnd, 0);
                s_previousFocusWnd = s_hwndCocos;
            }
          
            break;
        default:
            break;
        }

        return CallWindowProc(s_prevCocosWndProc, hwnd, uMsg, wParam, lParam);
    }

    LRESULT EditBoxImplWin::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        EditBoxImplWin* pThis = (EditBoxImplWin*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (pThis)
        {
            pThis->_WindowProc(hwnd, uMsg, wParam, lParam);
        }

        return CallWindowProc(pThis->_prevWndProc, hwnd, uMsg, wParam, lParam);

    }
}

NS_CC_END

#endif /* (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) */
