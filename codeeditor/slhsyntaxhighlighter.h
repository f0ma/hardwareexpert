#ifndef SLHSYNTAXHIGHLIGHTER_H
#define SLHSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegExp>

class SLHSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    SLHSyntaxHighlighter (QTextDocument *te) : QSyntaxHighlighter (te) {}

    virtual void highlightBlock (const QString& text)
    {
        int pos = 0;

        static QRegExp rgx ("%.*");

        if ((pos = rgx.indexIn (text, pos)) != -1)
        {
                setFormat (pos, text.length(), Qt::darkGreen);
                return;
        }

        setFormat (0, text.length(), Qt::black);

        static QRegExp rw_rgx ("\\b(?:if|while|for|do|define|def|defapi|var|variable|break|continue|try|expect|return)\\b");

        pos = 0;
        while ((pos = rw_rgx.indexIn (text, pos)) != -1)
        {
                setFormat (pos, rw_rgx.matchedLength(), Qt::darkYellow);
                pos += rw_rgx.matchedLength();
        }

        static QRegExp o_rgx ("(\".*\")|(\\b[0-9]+\\b)|(\\b0b[0-1]+\\b)|(\\b0x[0-9a-fA-F]+\\b)");
        o_rgx.setMinimal(true);

        pos = 0;
        while ((pos = o_rgx.indexIn (text, pos)) != -1)
        {
                setFormat (pos, o_rgx.matchedLength(), Qt::red);
                pos += o_rgx.matchedLength();
        }

        static QRegExp rw_rgx_const (
        "\\b(?:ByteArray|length|setBit|getBit|wait|clear|cr|print|progValue|progMax|progWait|progClear|optClear|optOption|optDialog|ask|"
        "fromInt|fromHex|fromAscii|concat|subArray|toString|copy|fill|load|pageLoad|pageOverwrite|pageStore|pageDelete|"
        "comOpen|comClose|comWrite|comRead|comRecived|comSetDtr|comSetRts|comSetTimeout|"
        "lptOpen|lptSetData|lptGetData|lptSetCtrl|lptGetStat|lptSetMode|lptClose|"
        "i2cOpen|i2cAddWrite|i2cAddRead|i2cAddStop|i2cAddWait|i2cWrite|i2cRead|i2cWriteRead|i2cAskCheck|i2cIsOk|i2cGetBuffer|i2cGetBufferLength|i2cPreform|i2cClose|"
        "dioSetPin|dioSetPin|dioSetPort|dioGetPort|dioSetPinMode|dioSetPortMode|"
        "usbOpen|usbClose|usbReset|usbBulkRead|usbBulkWrite|usbControlMsg|configLoad|configValue|"
        "TRUE|FALSE|OUT|IN|HEX|DEC|BIN|BAUD_9600|BAUD_19200|BAUD_115200|DATA_5|DATA_6|DATA_7|DATA_8|PAR_NONE"
        "PAR_ODD|PAR_EVEN|STOP_1|STOP_2|OPT_CHK|OPT_DEC|OPT_HEX|OPT_LIST|OPT_STR)\\b"
         );

        pos = 0;
        while ((pos = rw_rgx_const.indexIn (text, pos)) != -1)
        {
                setFormat (pos, rw_rgx_const.matchedLength(), Qt::darkBlue);
                pos += rw_rgx_const.matchedLength();
        }

    }
};

#endif // SLHSYNTAXHIGHLIGHTER_H
