/*
 * BabbleSynth
 * Copyright (C) 2021  Clo Yun-Hee Dufour
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BABBLESYNTH_CLICKABLE_LABEL_H
#define BABBLESYNTH_CLICKABLE_LABEL_H

#include <QtWidgets>

namespace babblesynth {
namespace gui {

class ClickableLabel : public QLabel {
    Q_OBJECT

   public:
    explicit ClickableLabel(QWidget *parent = nullptr,
                            Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();

   signals:
    void clicked(QMouseEvent *event);

   protected:
    void mousePressEvent(QMouseEvent *event) override;
};

}  // namespace gui
}  // namespace babblesynth

#if defined(_MSC_VER) && _MSC_VER > 1310 && !defined(__clang__)
#include <windows.h>
// Visual C++ 2005 and later require the source files in UTF-8, and all strings
// to be encoded as wchar_t otherwise the strings will be converted into the
// local multibyte encoding and cause errors. To use a wchar_t as UTF-8, these
// strings then need to be convert back to UTF-8. This function is just a rough
// example of how to do this.
#define utf8(str) ConvertToUTF8(L##str)
inline const char *ConvertToUTF8(const wchar_t *pStr) {
    static char szBuf[1024];
    WideCharToMultiByte(CP_UTF8, 0, pStr, -1, szBuf, sizeof(szBuf), NULL, NULL);
    return szBuf;
}
#else
// Visual C++ 2003 and gcc will use the string literals as is, so the files
// should be saved as UTF-8. gcc requires the files to not have a UTF-8 BOM.
#define utf8(str) str
#endif

#endif  // BABBLESYNTH_CLICKABLE_LABEL_H