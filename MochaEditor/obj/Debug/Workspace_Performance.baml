   M S B A M L   `   `   `  ����e  aDe.TorstenMandelkow.MetroChart, Version=1.0.0.0, Culture=neutral, PublicKeyToken=d5b8d1bbc99c4a2awTclr-namespace:De.TorstenMandelkow.MetroChart;assembly=De.TorstenMandelkow.MetroChartDe.TorstenMandelkow.MetroChart  5       MochaEditor'   !MochaEditor.Workspace_Performance   5      \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35` \RibbonControlsLibrary, Version=3.5.41019.1, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35H 9http://schemas.microsoft.com/winfx/2006/xaml/presentation      5      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5      Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5      @d2http://schemas.microsoft.com/expression/blend/2008     5      X TXceed.Wpf.Toolkit, Version=2.1.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f45xceed)http://schemas.xceed.com/wpf/xaml/toolkit  5      amchartTclr-namespace:De.TorstenMandelkow.MetroChart;assembly=De.TorstenMandelkow.MetroChart  5      -   5        $
��Auto��5	   J   $
��Auto��6W   .��5      ]� .+�5   
   �� $��Bottom=�6     �� Orientation$  
Horizontal=�6-   .+�5      �� $��Refresh Interval: ��6   $  Xceed.Wpf.Toolkit.DoubleUpDown  5      -   5        $��RefreshTimeComp��5   `   �  �Xceed.Wpf.Toolkit.NumericUpDown`1[[System.Nullable`1[[System.Double, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]], mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]]   	Increment$	 1.0X�6!   
������ $	��100��5   :   
��������  �Xceed.Wpf.Toolkit.Primitives.UpDownBase`1[[System.Nullable`1[[System.Double, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]], mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]]   Value 5   1   �� *RefreshInterval+ �� Mode$ TwoWay=�5   I   �� 5      $��20,0,0,0q�64   ��6   �� *
statusText+64   5      -   'De.TorstenMandelkow.MetroChart.PieChart  5   
   -   5        $��PerfPie��5      .   (De.TorstenMandelkow.MetroChart.ChartBase   
ChartTitle$ System Performance��6)      ChartSubTitle$ avg. running time in MS��6I   
������ 
�������   ChartLegendVisibility$ 	Collapsed=�5   K      Series 5      0   *De.TorstenMandelkow.MetroChart.ChartSeries     SeriesTitle$ SystemPerformance��5      	   DisplayMember$	 
SystemName��5      
   ValueMember$
 
AvgRunTime��5      ��5      �� *PerfTimeData+5      9   3De.TorstenMandelkow.MetroChart.ClusteredColumnChart  5   
   
������ 
�������   ChartTitleVisibility$ Hidden=�6W    5        $ SubSystem Breakdown��5      $	 
SystemName��5       $
 
AvgRunTime��5!      ��5"      �� *PerfTimeData+5$      5%      5'      