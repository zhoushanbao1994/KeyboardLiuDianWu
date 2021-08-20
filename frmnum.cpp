#include "frmnum.h"
#include "ui_frmnum.h"
#include "qdesktopwidget.h"

frmNum *frmNum::_m_instance = 0;
frmNum::frmNum(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmNum)
{
    ui->setupUi(this);
    this->InitForm();
    this->InitProperty();
    this->ChangeStyle();
}

frmNum::~frmNum()
{
    delete ui;
}

void frmNum::Init(QString style, int fontSize) {
    this->m_currentStyle = style;
    this->m_currentFontSize = fontSize;
    this->ChangeStyle();
}

void frmNum::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - m_mousePoint);
        e->accept();
    }
}

void frmNum::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_mousePressed = true;
        m_mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void frmNum::mouseReleaseEvent(QMouseEvent *)
{
    m_mousePressed = false;
}

void frmNum::InitForm()
{
    this->m_mousePressed = false;
    this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    QDesktopWidget w;
    m_deskWidth = w.availableGeometry().width();
    m_deskHeight = w.availableGeometry().height();
    m_frmWidth = this->width();
    m_frmHeight = this->height();

    m_isFirst = true;
    m_isPress = false;
    m_timerPress = new QTimer(this);
    connect(m_timerPress, SIGNAL(timeout()), this, SLOT(reClicked()));
    m_currentWidget = 0;

    //如果需要更改输入法面板的样式,改变currentStyle这个变量即可
    //blue--淡蓝色  dev--dev风格  black--黑色  brown--灰黑色  lightgray--浅灰色  darkgray--深灰色  gray--灰色  silvery--银色
    m_currentStyle = "";

    //输入法面板字体大小,如果需要更改面板字体大小,该这里即可
    m_currentFontSize = 10;

    QList<QPushButton *> btn = this->findChildren<QPushButton *>();
    foreach (QPushButton * b, btn) {
        connect(b, SIGNAL(clicked()), this, SLOT(btn_clicked()));
    }

    //绑定全局改变焦点信号槽
    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)),
            this, SLOT(focusChanged(QWidget *, QWidget *)));
    //绑定按键事件过滤器
    qApp->installEventFilter(this);
}

void frmNum::InitProperty()
{
    ui->btn0->setProperty("btnNum", true);
    ui->btn1->setProperty("btnNum", true);
    ui->btn2->setProperty("btnNum", true);
    ui->btn3->setProperty("btnNum", true);
    ui->btn4->setProperty("btnNum", true);
    ui->btn5->setProperty("btnNum", true);
    ui->btn6->setProperty("btnNum", true);
    ui->btn7->setProperty("btnNum", true);
    ui->btn8->setProperty("btnNum", true);
    ui->btn9->setProperty("btnNum", true);
    ui->btn00->setProperty("btnNum", true);

    ui->btnDot->setProperty("btnOther", true);
    ui->btnSpace->setProperty("btnOther", true);
    ui->btnDelete->setProperty("btnOther", true);
}

void frmNum::ShowPanel()
{
    this->setVisible(true);
}

void frmNum::focusChanged(QWidget *oldWidget, QWidget *nowWidget)
{
    //qDebug() << "oldWidget:" << oldWidget << " nowWidget:" << nowWidget;
    if (nowWidget != 0 && !this->isAncestorOf(nowWidget)) {
        //在Qt5和linux系统中(嵌入式linux除外),当输入法面板关闭时,焦点会变成无,然后焦点会再次移到焦点控件处
        //这样导致输入法面板的关闭按钮不起作用,关闭后马上有控件获取焦点又显示.
        //为此,增加判断,当焦点是从有对象转为无对象再转为有对象时不要显示.
        //这里又要多一个判断,万一首个窗体的第一个焦点就是落在可输入的对象中,则要过滤掉
#ifndef __arm__
        if (oldWidget == 0x0 && !m_isFirst) {
            QTimer::singleShot(0, this, SLOT(hide()));
            return;
        }
#endif

        //如果对应属性noinput为真则不显示
        if (nowWidget->property("noinput").toBool()) {
            QTimer::singleShot(0, this, SLOT(hide()));
            return;
        }

        m_isFirst = false;
        if (nowWidget->inherits("QLineEdit")) {
            m_currentLineEdit = (QLineEdit *)nowWidget;
            m_currentEditType = "QLineEdit";
            ShowPanel();
        } else if (nowWidget->inherits("QTextEdit")) {
            m_currentTextEdit = (QTextEdit *)nowWidget;
            m_currentEditType = "QTextEdit";
            ShowPanel();
        } else if (nowWidget->inherits("QPlainTextEdit")) {
            m_currentPlain = (QPlainTextEdit *)nowWidget;
            m_currentEditType = "QPlainTextEdit";
            ShowPanel();
        } else if (nowWidget->inherits("QTextBrowser")) {
            m_currentBrowser = (QTextBrowser *)nowWidget;
            m_currentEditType = "QTextBrowser";
            ShowPanel();
        } else if (nowWidget->inherits("QComboBox")) {
            QComboBox *cbox = (QComboBox *)nowWidget;
            //只有当下拉选择框处于编辑模式才可以输入
            if (cbox->isEditable()) {
                m_currentLineEdit = cbox->lineEdit() ;
                m_currentEditType = "QLineEdit";
                ShowPanel();
            }
        } else if (nowWidget->inherits("QSpinBox") ||
                   nowWidget->inherits("QDoubleSpinBox") ||
                   nowWidget->inherits("QDateEdit") ||
                   nowWidget->inherits("QTimeEdit") ||
                   nowWidget->inherits("QDateTimeEdit")) {
            m_currentWidget = nowWidget;
            m_currentEditType = "QWidget";
            ShowPanel();
        } else {
            m_currentWidget = 0;
            m_currentLineEdit = 0;
            m_currentTextEdit = 0;
            m_currentPlain = 0;
            m_currentBrowser = 0;
            m_currentEditType = "";
            this->setVisible(false);
        }

        QRect rect = nowWidget->rect();
        QPoint pos = QPoint(rect.left(), rect.bottom() + 2);
        pos = nowWidget->mapToGlobal(pos);

        int x = pos.x();
        int y = pos.y();
        if (pos.x() + m_frmWidth > m_deskWidth) {
            x = m_deskWidth - m_frmWidth;
        }
        if (pos.y() + m_frmHeight > m_deskHeight) {
            y = pos.y() - m_frmHeight - rect.height() - 35;
        }

        this->setGeometry(x, y, m_frmWidth, m_frmHeight);
    }
}

bool frmNum::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        if (m_currentEditType != "") {
            if (obj != ui->btnClose) {
                QString objName = obj->objectName();
                if (!obj->property("noinput").toBool() && objName != "frmMainWindow"
                        && objName != "frmInputWindow" && objName != "qt_edit_menu") {
                    ShowPanel();
                }
            }
            m_btnPress = (QPushButton *)obj;
            if (checkPress()) {
                m_isPress = true;
                m_timerPress->start(500);
            }
        }
        return false;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        m_btnPress = (QPushButton *)obj;
        if (checkPress()) {
            m_isPress = false;
            m_timerPress->stop();
        }
        return false;
    }
    return QWidget::eventFilter(obj, event);
}

bool frmNum::checkPress()
{
    //只有属于数字键盘的合法按钮才继续处理
    bool num_ok = m_btnPress->property("btnNum").toBool();
    bool other_ok = m_btnPress->property("btnOther").toBool();
    if (num_ok || other_ok) {
        return true;
    }
    return false;
}

void frmNum::reClicked()
{
    if (m_isPress) {
        m_timerPress->setInterval(30);
        m_btnPress->click();
    }
}

void frmNum::btn_clicked()
{
    //如果当前焦点控件类型为空,则返回不需要继续处理
    if (m_currentEditType == "") {
        return;
    }

    QPushButton *btn = (QPushButton *)sender();
    QString objectName = btn->objectName();
    if (objectName == "btnDelete") {
        deleteValue();
    } else if (objectName == "btnClose") {
        this->setVisible(false);
    } else if (objectName == "btnEnter") {
        this->setVisible(false);
    } else if (objectName == "btnSpace") {
        insertValue(" ");
    } else {
        QString value = btn->text();
        insertValue(value);
    }
}

void frmNum::insertValue(QString value)
{
    if (m_currentEditType == "QLineEdit") {
        m_currentLineEdit->insert(value);
    } else if (m_currentEditType == "QTextEdit") {
        m_currentTextEdit->insertPlainText(value);
    } else if (m_currentEditType == "QPlainTextEdit") {
        m_currentPlain->insertPlainText(value);
    } else if (m_currentEditType == "QTextBrowser") {
        m_currentBrowser->insertPlainText(value);
    } else if (m_currentEditType == "QWidget") {
        QKeyEvent keyPress(QEvent::KeyPress, 0, Qt::NoModifier, QString(value));
        QApplication::sendEvent(m_currentWidget, &keyPress);
    }
}

void frmNum::deleteValue()
{
    if (m_currentEditType == "QLineEdit") {
        m_currentLineEdit->backspace();
    } else if (m_currentEditType == "QTextEdit") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = m_currentTextEdit->textCursor();
        if(cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (m_currentEditType == "QPlainTextEdit") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = m_currentPlain->textCursor();
        if(cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (m_currentEditType == "QTextBrowser") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = m_currentBrowser->textCursor();
        if(cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (m_currentEditType == "QWidget") {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier, QString());
        QApplication::sendEvent(m_currentWidget, &keyPress);
    }
}

void frmNum::ChangeStyle()
{
    if (m_currentStyle == "blue") {
        changeStyle("#DEF0FE", "#C0DEF6", "#C0DCF2", "#386487");
    } else if (m_currentStyle == "dev") {
        changeStyle("#C0D3EB", "#BCCFE7", "#B4C2D7", "#324C6C");
    } else if (m_currentStyle == "gray") {
        changeStyle("#E4E4E4", "#A2A2A2", "#A9A9A9", "#000000");
    } else if (m_currentStyle == "lightgray") {
        changeStyle("#EEEEEE", "#E5E5E5", "#D4D0C8", "#6F6F6F");
    } else if (m_currentStyle == "darkgray") {
        changeStyle("#D8D9DE", "#C8C8D0", "#A9ACB5", "#5D5C6C");
    } else if (m_currentStyle == "black") {
        changeStyle("#4D4D4D", "#292929", "#D9D9D9", "#CACAD0");
    } else if (m_currentStyle == "brown") {
        changeStyle("#667481", "#566373", "#C2CCD8", "#E7ECF0");
    } else if (m_currentStyle == "silvery") {
        changeStyle("#E1E4E6", "#CCD3D9", "#B2B6B9", "#000000");
    }
}

void frmNum::changeStyle(QString topColor, QString bottomColor, QString borderColor, QString textColor)
{
    QStringList qss;
    qss.append(QString("QWidget#widget_title{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}")
               .arg(topColor).arg(bottomColor));
    qss.append("QPushButton{padding:5px;border-radius:3px;}");
    qss.append(QString("QPushButton:hover{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}")
               .arg(topColor).arg(bottomColor));
    qss.append(QString("QLabel,QPushButton{font-size:%1pt;color:%2;}")
               .arg(m_currentFontSize).arg(textColor));
    qss.append(QString("QPushButton#btnPre,QPushButton#btnNext,QPushButton#btnClose{padding:5px;}"));
    qss.append(QString("QPushButton{border:1px solid %1;background:rgba(0,0,0,0);}")
               .arg(borderColor));
    qss.append(QString("QLineEdit{border:1px solid %1;border-radius:5px;padding:2px;background:none;selection-background-color:%2;selection-color:%3;}")
               .arg(borderColor).arg(bottomColor).arg(topColor));
    this->setStyleSheet(qss.join(""));
}
