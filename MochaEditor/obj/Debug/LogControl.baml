   M S B A M L   `   `   `  ����  MochaEditor    MochaEditor.LogControl   5      \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35` \RibbonControlsLibrary, Version=3.5.41019.1, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35H 9http://schemas.microsoft.com/winfx/2006/xaml/presentation      5      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5      Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5      @d2http://schemas.microsoft.com/expression/blend/2008     5      .��5      ]� .+�5	   
   �� -   5        $��ClearButton��5	   4   $��Right=�6   $��X��6(   $��40��6a   $��9(�6o   &   System.Windows.Controls.DataGrid  5
   
   -   5        $��ConsoleDataGrid��5   :       AutoGenerateColumns  .  ��. ! �� IsDeferredScrollingEnabled .    CanUserAddRows .     CanUserDeleteRows .   %� 
ScrollUnit$ Pixel=�5   .   ��5
      ��  �� RelativeSource ��  �� AncestorType" >� �� Path$
 logs�   Columns 5      0  *System.Windows.Controls.DataGridTextColumn  5      ,  &System.Windows.Controls.DataGridColumn	   Header$
	 Time��6%   1  +System.Windows.Controls.DataGridBoundColumn
   Binding
 63   �� *Time+   ElementStyle 5      �� 5      .�5      ��  �� Property � �� �� Value$
 Wrap=�6G   5      5        5      $
	 Text��6%   
 63   �� *text+ 5      �� 5      .�5      ��  � ��$
 Wrap=�6G   5      5      5      5      5       