   M S B A M L   `   `   `  ���� e  aDe.TorstenMandelkow.MetroChart, Version=1.0.0.0, Culture=neutral, PublicKeyToken=d5b8d1bbc99c4a2awTclr-namespace:De.TorstenMandelkow.MetroChart;assembly=De.TorstenMandelkow.MetroChartDe.TorstenMandelkow.MetroChart   MochaEditor'   !MochaEditor.Workspace_Performance   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35` \RibbonControlsLibrary, Version=3.5.41019.1, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35H 9http://schemas.microsoft.com/winfx/2006/xaml/presentation      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     @d2http://schemas.microsoft.com/expression/blend/2008     X TXceed.Wpf.Toolkit, Version=2.1.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f45xceed)http://schemas.xceed.com/wpf/xaml/toolkit  amchartTclr-namespace:De.TorstenMandelkow.MetroChart;assembly=De.TorstenMandelkow.MetroChart  -   $
��Auto��$
��Auto��.��]� .+��� $��Bottom=�  �� Orientation$  
Horizontal=�.+��� $��Refresh Interval: ��$  Xceed.Wpf.Toolkit.DoubleUpDown  -   $��RefreshTimeComp���  �Xceed.Wpf.Toolkit.NumericUpDown`1[[System.Nullable`1[[System.Double, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]], mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]]   	Increment$	 1.0X�
������ $	��100��
��������  �Xceed.Wpf.Toolkit.Primitives.UpDownBase`1[[System.Nullable`1[[System.Double, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]], mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]]   Value �� *RefreshInterval+ �� Mode$ TwoWay=��� $��20,0,0,0q����� *
statusText+-   'De.TorstenMandelkow.MetroChart.PieChart  -   $��PerfPie��.   (De.TorstenMandelkow.MetroChart.ChartBase   
ChartTitle$ System Performance��   ChartSubTitle$ avg. running time in MS��
������ 
�������   ChartLegendVisibility$ 	Collapsed=�   Series 0   *De.TorstenMandelkow.MetroChart.ChartSeries     SeriesTitle$ SystemPerformance��	   DisplayMember$	 
SystemName��
   ValueMember$
 
AvgRunTime������ *PerfTimeData+9   3De.TorstenMandelkow.MetroChart.ClusteredColumnChart  
������ 
�������   ChartTitleVisibility$ Hidden=�   $ SubSystem Breakdown��$	 
SystemName��$
 
AvgRunTime������ *PerfTimeData+