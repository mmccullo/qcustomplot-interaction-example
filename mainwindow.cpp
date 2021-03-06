#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	srand(QDateTime::currentDateTime().toTime_t());
	ui->setupUi(this);

	setMinimumWidth(1024);
	setMinimumHeight(768);

	ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
		QCP::iSelectLegend | QCP::iSelectPlottables);

	// title
	ui->customPlot->plotLayout()->insertRow(0);
	QCPTextElement *title = new QCPTextElement(ui->customPlot, "Independent Axes Interaction Example", QFont("sans", 17, QFont::Bold));
	ui->customPlot->plotLayout()->addElement(0, 0, title);
	
	// connect slot that ties tick marks and axes selections together for each axis:
	connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

	// connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
	connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
	connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

	// make bottom and left axes transfer their ranges to top and right axes:
	//connect(ui->customPlot->xAxis2, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
	//connect(ui->customPlot->yAxis2, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

	// connect some interaction slots:
	connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis*, QCPAxis::SelectablePart, QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*, QCPAxis::SelectablePart)));
	connect(ui->customPlot, SIGNAL(legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*)));
	connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));

	// connect slot that shows a message in the status bar when a graph is clicked:
	connect(ui->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*, int)));

	// setup policy and connect slot for context menu popup:
	ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

	ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedStates)); // period as decimal separator and comma as thousand separator
	ui->customPlot->legend->setVisible(true);
	QFont legendFont = font();  // start out with MainWindow's font..
	legendFont.setPointSize(9); // and make a bit smaller for legend
	ui->customPlot->legend->setFont(legendFont);
	ui->customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
	// by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
	ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

	// setup for graph 0: key axis left, value axis bottom
	// will contain left maxwell-like function
	ui->customPlot->addGraph(ui->customPlot->yAxis, ui->customPlot->xAxis);
	ui->customPlot->graph(0)->setPen(QPen(QColor(255, 100, 0)));
	ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
	ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
	ui->customPlot->graph(0)->setName("Left maxwell function");

	// setup for graph 1: key axis bottom, value axis left (those are the default axes)
	// will contain bottom maxwell-like function with error bars
	ui->customPlot->addGraph();
	ui->customPlot->graph(1)->setPen(QPen(Qt::red));
	ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
	ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));
	ui->customPlot->graph(1)->setName("Bottom maxwell function");

	// setup for graph 2: key axis top, value axis right
	// will contain high frequency sine with low frequency beating:
	ui->customPlot->addGraph(ui->customPlot->xAxis2, ui->customPlot->yAxis2);
	ui->customPlot->graph(2)->setPen(QPen(Qt::blue));
	ui->customPlot->graph(2)->setName("High frequency sine");

	// setup for graph 3: same axes as graph 2
	// will contain low frequency beating envelope of graph 2
	ui->customPlot->addGraph(ui->customPlot->xAxis2, ui->customPlot->yAxis2);
	QPen blueDotPen;
	blueDotPen.setColor(QColor(30, 40, 255, 150));
	blueDotPen.setStyle(Qt::DotLine);
	blueDotPen.setWidthF(4);
	ui->customPlot->graph(3)->setPen(blueDotPen);
	ui->customPlot->graph(3)->setName("Sine envelope");

	// setup for graph 4: key axis right, value axis top
	// will contain parabolically distributed data points with some random perturbance
	ui->customPlot->addGraph(ui->customPlot->yAxis2, ui->customPlot->xAxis2);
	ui->customPlot->graph(4)->setPen(QColor(50, 50, 50, 255));
	ui->customPlot->graph(4)->setLineStyle(QCPGraph::lsNone);
	ui->customPlot->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
	ui->customPlot->graph(4)->setName("Some random data around\na quadratic function");

	// generate data, just playing with numbers, not much to learn here:
	QVector<double> x0(25), y0(25);
	QVector<double> x1(15), y1(15);
	QVector<double> x2(250), y2(250);
	QVector<double> x3(250), y3(250);
	QVector<double> x4(250), y4(250);

	for (int i = 0; i < 25; ++i) // data for graph 0
	{
		x0[i] = 3 * i / 25.0;
		y0[i] = qExp(-x0[i] * x0[i] * 0.8)*(x0[i] * x0[i] + x0[i]);
	}

	for (int i = 0; i < 15; ++i) // data for graph 1
	{
		x1[i] = 3 * i / 15.0;;
		y1[i] = qExp(-x1[i] * x1[i])*(x1[i] * x1[i])*2.6;
	}

	for (int i = 0; i < 250; ++i) // data for graphs 2, 3 and 4
	{
		x2[i] = i / 250.0 * 3 * M_PI;
		x3[i] = x2[i];
		x4[i] = i / 250.0 * 100 - 50;
		y2[i] = qSin(x2[i] * 12)*qCos(x2[i]) * 10;
		y3[i] = qCos(x3[i]) * 10;
		y4[i] = 0.01*x4[i] * x4[i] + 1.5*(rand() / (double)RAND_MAX - 0.5) + 1.5*M_PI;
	}

	// pass data points to graphs:
	ui->customPlot->graph(0)->setData(x0, y0);
	ui->customPlot->graph(1)->setData(x1, y1);
	ui->customPlot->graph(2)->setData(x2, y2);
	ui->customPlot->graph(3)->setData(x3, y3);
	ui->customPlot->graph(4)->setData(x4, y4);

	// activate top and right axes, which are invisible by default:
	ui->customPlot->xAxis2->setVisible(true);
	ui->customPlot->yAxis2->setVisible(true);

	// set ranges appropriate to show data:
	ui->customPlot->xAxis->setRange(0, 2.7);
	ui->customPlot->yAxis->setRange(0, 2.6);
	ui->customPlot->xAxis2->setRange(0, 3.0*M_PI);
	ui->customPlot->yAxis2->setRange(-70, 35);

	// set pi ticks on top axis:
	ui->customPlot->xAxis2->setTicker(QSharedPointer<QCPAxisTickerPi>(new QCPAxisTickerPi));
	
	// set labels:
	ui->customPlot->xAxis->setLabel("Bottom axis with outward ticks");
	ui->customPlot->yAxis->setLabel("Left axis label");
	ui->customPlot->xAxis2->setLabel("Top axis label");
	ui->customPlot->yAxis2->setLabel("Right axis label");

	// make ticks on bottom axis go outward:
	ui->customPlot->xAxis->setTickLength(0, 5);
	ui->customPlot->xAxis->setSubTickLength(0, 3);

	// make ticks on right axis go inward and outward:
	ui->customPlot->yAxis2->setTickLength(3, 3);
	ui->customPlot->yAxis2->setSubTickLength(1, 1);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::titleDoubleClick(QMouseEvent* event)
{
	Q_UNUSED(event)
		if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
		{
			// Set the plot title by double clicking on it
			bool ok;
			QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
			if (ok)
			{
				title->setText(newTitle);
				ui->customPlot->replot();
			}
		}
}

void MainWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
	// Set an axis label by double clicking on it
	if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
	{
		bool ok;
		QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
		if (ok)
		{
			axis->setLabel(newLabel);
			ui->customPlot->replot();
		}
	}
}

void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
	// Rename a graph by double clicking on its legend item
	Q_UNUSED(legend)
		if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
		{
			QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
			bool ok;
			QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
			if (ok)
			{
				plItem->plottable()->setName(newName);
				ui->customPlot->replot();
			}
		}
}

void MainWindow::selectionChanged()
{
	/*
	 normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
	 the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
	 and the axis base line together. However, the axis label shall be selectable individually.

	 Further, we want to synchronize the selection of the graphs with the selection state of the respective
	 legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
	 or on its legend item.
	*/

	// handle axis and tick labels as one selectable object:
	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
		ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);

	if (ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
		ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);

	if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
		ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);

	if (ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
		ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);

	// synchronize selection of graphs with selection of corresponding legend items:
	for (int i = 0; i < ui->customPlot->graphCount(); ++i)
	{
		QCPGraph *graph = ui->customPlot->graph(i);
		QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
		if (item->selected() || graph->selected())
		{
			item->setSelected(true);
			graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
		}
	}
}

void MainWindow::mousePress()
{
	QList<QCPAxis*> axesList;

	// if an axis is selected, only allow the direction of that axis to be dragged
	// if no axis is selected, check if a graph is selected and drag both axes

	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
	{
		axesList.append(ui->customPlot->xAxis);
	}
	else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
	{
		axesList.append(ui->customPlot->yAxis);
	}
	else if (ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis))
	{
		axesList.append(ui->customPlot->xAxis2);
	}
	else if (ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis))
	{
		axesList.append(ui->customPlot->yAxis2);
	}
	else
	{
		bool selectedGraph = false;

		// is a graph selected?
		for (int i = 0; i < ui->customPlot->graphCount(); ++i)
		{
			QCPGraph *graph = ui->customPlot->graph(i);
			if (graph->selected())
			{
				selectedGraph = true;
				axesList.append(graph->keyAxis());
				axesList.append(graph->valueAxis());
				break;
			}
		}

		if (!selectedGraph)
		{
			axesList.append(ui->customPlot->xAxis);
			axesList.append(ui->customPlot->yAxis);
		}
	}

	// set the axes to drag
	ui->customPlot->axisRect()->setRangeDragAxes(axesList);
}

void MainWindow::mouseWheel()
{
	QList<QCPAxis*> axesList;

	// if an axis is selected, only allow the direction of that axis to be zoomed
	// if no axis is selected, check if a graph is selected and zoom both axes

	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
	{
		axesList.append(ui->customPlot->xAxis);
	}
	else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
	{
		axesList.append(ui->customPlot->yAxis);
	}
	else if (ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis))
	{
		axesList.append(ui->customPlot->xAxis2);
	}
	else if (ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis))
	{
		axesList.append(ui->customPlot->yAxis2);
	}
	else
	{
		bool selectedGraph = false;

		// is a graph selected?
		for (int i = 0; i < ui->customPlot->graphCount(); ++i)
		{
			QCPGraph *graph = ui->customPlot->graph(i);
			if (graph->selected())
			{
				selectedGraph = true;
				axesList.append(graph->keyAxis());
				axesList.append(graph->valueAxis());
				break;
			}
		}

		if (!selectedGraph)
		{
			axesList.append(ui->customPlot->xAxis);
			axesList.append(ui->customPlot->yAxis);
		}
	}

	// set the axes to zoom
	ui->customPlot->axisRect()->setRangeZoomAxes(axesList);
}

void MainWindow::addRandomGraph()
{
	int n = 50; // number of points in graph
	double xScale = (rand() / (double)RAND_MAX + 0.5) * 2;
	double yScale = (rand() / (double)RAND_MAX + 0.5) * 2;
	double xOffset = (rand() / (double)RAND_MAX - 0.5) * 4;
	double yOffset = (rand() / (double)RAND_MAX - 0.5) * 10;
	double r1 = (rand() / (double)RAND_MAX - 0.5) * 2;
	double r2 = (rand() / (double)RAND_MAX - 0.5) * 2;
	double r3 = (rand() / (double)RAND_MAX - 0.5) * 2;
	double r4 = (rand() / (double)RAND_MAX - 0.5) * 2;
	QVector<double> x(n), y(n);
	for (int i = 0; i < n; i++)
	{
		x[i] = (i / (double)n - 0.5)*10.0*xScale + xOffset;
		y[i] = (qSin(x[i] * r1 * 5)*qSin(qCos(x[i] * r2)*r4 * 3) + r3*qCos(qSin(x[i])*r4 * 2))*yScale + yOffset;
	}

	ui->customPlot->addGraph();
	ui->customPlot->graph()->setName(QString("New graph %1").arg(ui->customPlot->graphCount() - 1));
	ui->customPlot->graph()->setData(x, y);
	ui->customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(rand() % 5 + 1));
	if (rand() % 100 > 50)
		ui->customPlot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand() % 14 + 1)));
	QPen graphPen;
	graphPen.setColor(QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10));
	graphPen.setWidthF(rand() / (double)RAND_MAX * 2 + 1);
	ui->customPlot->graph()->setPen(graphPen);
	ui->customPlot->replot();
}

void MainWindow::removeSelectedGraph()
{
	if (ui->customPlot->selectedGraphs().size() > 0)
	{
		ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
		ui->customPlot->replot();
	}
}

void MainWindow::removeAllGraphs()
{
	ui->customPlot->clearGraphs();
	ui->customPlot->replot();
}

void MainWindow::contextMenuRequest(QPoint pos)
{
	QMenu *menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	if (ui->customPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
	{
		menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignLeft));
		menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignHCenter));
		menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignRight));
		menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignRight));
		menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignLeft));
	}
	else  // general context menu on graphs requested
	{
		menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
		if (ui->customPlot->selectedGraphs().size() > 0)
			menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
		if (ui->customPlot->graphCount() > 0)
			menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
	}

	menu->popup(ui->customPlot->mapToGlobal(pos));
}

void MainWindow::moveLegend()
{
	if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
	{
		bool ok;
		int dataInt = contextAction->data().toInt(&ok);
		if (ok)
		{
			ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
			ui->customPlot->replot();
		}
	}
}

void MainWindow::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
	// since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
	// usually it's better to first check whether interface1D() returns non-zero, and only then use it.
	double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
	QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
	ui->statusBar->showMessage(message, 2500);
}