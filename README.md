# qcustomplot-interaction-example
Independent axis drag and zoom example using QCustomPlot

This example uses QCustomPlot (v2.0.0 or later) to create a plot with four independent axes for dragging and zooming. It is a slightly modified version of the interaction-example provided with the QCustomPlot distribution using some multiple graph/four axes example code copied from the QCustomPlot website.

The key in allowing independent dragging and zooming of each axis (left, right, bottom, and top) is the use of setRangeDragAxes() and setRangeZoomAxes() limited to the list of axes you wish to drag or zoom.

In addition to the original .pro file, a solution file is also provided that is compatible with Visual Studio 2015.
