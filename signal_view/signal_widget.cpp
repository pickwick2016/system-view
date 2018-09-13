#include <QKeyEvent>

#include "signal_widget.h"
#include "misc.h"


SignalWidget::SignalWidget(QWidget * parent) : QWidget(parent)
{
	initShortcuts();
}


void SignalWidget::initShortcuts()
{
	m_shortcuts.clear();

	// 1.初始化快捷键.
	m_shortcuts[XY_Reset] = { Qt::Key_Q, false, false, false };
	m_shortcuts[X_Reset] = { Qt::Key_Q, false, true, false };
	m_shortcuts[Y_Reset] = { Qt::Key_Q, true, false, false };

	m_shortcuts[X_Backward] = { Qt::Key_Left, false, false, false };
	m_shortcuts[X_Forward] = { Qt::Key_Right, false, false, false };
	m_shortcuts[X_ZoomIn] = { Qt::Key_Left, true, false, false };
	m_shortcuts[X_ZoomOut] = { Qt::Key_Right, true, false, false };


	m_shortcuts[Y_Backward] = { Qt::Key_Down, false, false, false };
	m_shortcuts[Y_Forward] = { Qt::Key_Up, false, false, false };
	m_shortcuts[Y_ZoomIn] = { Qt::Key_Down, true, false, false };
	m_shortcuts[Y_ZoomOut] = { Qt::Key_Up, true, false, false };

	// 2.初始化命令参数.
	m_commandParams[X_Backward] = QVariant(0.05);
	m_commandParams[X_Forward] = QVariant(0.05);
	m_commandParams[X_ZoomIn] = QVariant(0.05);
	m_commandParams[X_ZoomOut] = QVariant(0.05);
	m_commandParams[X_ZoomInAt] = QVariant(0.05);
	m_commandParams[X_ZoomOutAt] = QVariant(0.05);

	m_commandParams[Y_Backward] = QVariant(0.05);
	m_commandParams[Y_Forward] = QVariant(0.05);
	m_commandParams[Y_ZoomIn] = QVariant(0.05);
	m_commandParams[Y_ZoomOut] = QVariant(0.05);
	m_commandParams[Y_ZoomInAt] = QVariant(0.05);
	m_commandParams[Y_ZoomOutAt] = QVariant(0.05);
}

bool SignalWidget::executeCommand(int cmd, QVariant param)
{
	QRectF visible = visibleArea();
	QRectF total = totalArea();

	switch (cmd) {
	case SignalCommand::XY_Reset:
		setVisibleArea(totalArea());
		break;
	case SignalCommand::X_Reset:
		visible.setLeft(total.left());
		visible.setRight(total.right());
		setVisibleArea(visible);
		break;
	case SignalCommand::Y_Reset:
		visible.setTop(total.top());
		visible.setBottom(total.bottom());
		setVisibleArea(visible);
		break;
	case SignalCommand::X_Backward:
		visible = tool::RectMoveX(visible, - visible.width() * param.toDouble());
		visible = tool::RectAdjustX(visible, total, true);
		setVisibleArea(visible);
		break;
	case SignalCommand::X_Forward:
		visible = tool::RectMoveX(visible, visible.width() * param.toDouble());
		visible = tool::RectAdjustX(visible, total, true);
		setVisibleArea(visible);
		break;
	case SignalCommand::X_ZoomIn:
		visible = tool::RectExpandX(visible, visible.width() * param.toDouble());
		setVisibleArea(visible);
		break;
	case SignalCommand::X_ZoomOut:
		visible = tool::RectExpandX(visible, -visible.width() * param.toDouble());
		setVisibleArea(visible);
		break;
	case SignalCommand::X_ZoomInAt:
		visible = tool::RectExpandX(visible, visible.width() * 0.1);
		setVisibleArea(visible);
		break;
	case SignalCommand::X_ZoomOutAt:
		visible = tool::RectExpandX(visible, -visible.width() * 0.1);
		setVisibleArea(visible);
		break;
	case SignalCommand::Y_Backward:
		visible = tool::RectMoveY(visible, -visible.height() * param.toDouble());
		visible = tool::RectAdjustY(visible, total, true);
		setVisibleArea(visible);
		break;
	case SignalCommand::Y_Forward:
		visible = tool::RectMoveY(visible, visible.height() * param.toDouble());
		visible = tool::RectAdjustY(visible, total, true);
		setVisibleArea(visible);
		break;
	case SignalCommand::Y_ZoomIn:
		visible = tool::RectExpandY(visible, visible.height() * param.toDouble());
		setVisibleArea(visible);
		break;
	case SignalCommand::Y_ZoomOut:
		visible = tool::RectExpandY(visible, -visible.height() * param.toDouble());
		setVisibleArea(visible);
		break;
	case SignalCommand::Y_ZoomInAt:
		visible = tool::RectExpandY(visible, visible.height() * 0.1);
		setVisibleArea(visible);
		break; 
	case SignalCommand::Y_ZoomOutAt:
		visible = tool::RectExpandY(visible, -visible.height() * 0.1);
		setVisibleArea(visible);
		break;
	default:
		return false;
	}

	return true;
}


SignalWidget::KeyState SignalWidget::makeKeyState(QKeyEvent * evt)
{
	assert(evt != nullptr);

	bool ctrl = evt->modifiers() & Qt::ControlModifier;
	bool shift = evt->modifiers() & Qt::ShiftModifier;
	bool alt = evt->modifiers() & Qt::AltModifier;

	KeyState keystate = { evt->key(), ctrl, shift, alt };

	return keystate;
}

void SignalWidget::keyPressEvent(QKeyEvent * evt)
{
	KeyState ks = makeKeyState(evt);

	auto fit = std::find_if(m_shortcuts.begin(), m_shortcuts.end(), 
		[=](auto it) { return it.second == ks; });

	if (fit != m_shortcuts.end()) {
		int cmd = fit->first;
		QVariant param = tool::safe_get(m_commandParams, cmd, QVariant());
		executeCommand(cmd, m_commandParams[cmd]);
		
		evt->accept();
	}
}

QRectF SignalWidget::checkPointBound(QWheelEvent * evt)
{
	assert(evt != nullptr);

	auto pos = evt->pos();
	if (this->viewport(10).contains(pos)) {
		return viewport(10);
	}
	else if (viewport(20).contains(pos)) {
		return viewport(20);
	}

	return QRectF();
}

void SignalWidget::wheelEvent(QWheelEvent * evt)
{
	auto bound = checkPointBound(evt);
	if (bound.isEmpty()) {
		return;
	}

	bool ctrl = evt->modifiers() & Qt::ControlModifier;
	auto pos = evt->pos();
	auto pxy = tool::Map(pos, bound, tool::RectFlipY(visibleArea()));
	auto angles = evt->angleDelta();
	if (!angles.isNull()) {
		if (ctrl) {
			if (angles.ry() > 0) {
				executeCommand(Y_ZoomInAt, pxy.ry());
			}
			else {
				executeCommand(Y_ZoomOutAt, pxy.ry());
			}
		}
		else {
			if (angles.ry() > 0) {
				executeCommand(X_ZoomInAt, pxy.rx());
			}
			else {
				executeCommand(X_ZoomOutAt, pxy.rx());
			}
		}
		evt->accept();
	}
}

QRectF SignalWidget::visibleArea()
{
	return m_visibleArea;
}


void SignalWidget::setVisibleArea(QRectF r)
{
	m_visibleArea = r;
}

QRectF SignalWidget::totalArea()
{
	return QRectF();
}


