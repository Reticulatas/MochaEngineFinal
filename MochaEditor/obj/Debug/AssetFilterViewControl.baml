   M S B A M L   `   `   `  ����F  BMochaEditor, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null)clr-namespace:MochaEditorMochaEditor  5      (    "MochaEditor.AssetFilterViewControl   5      \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35` \RibbonControlsLibrary, Version=3.5.41019.1, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35H 9http://schemas.microsoft.com/winfx/2006/xaml/presentation      5      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5      Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5      @d2http://schemas.microsoft.com/expression/blend/2008     5      +mochaclr-namespace:MochaEditor     5      -   5          � DataContext  5   7   ��  �� RelativeSource �� *Self+ � 	Resources 5       �� MergedDictionaries �� Source   DetailsPanelTemplate � IsSharedSizeScope  GridPanelTemplate � 	ItemWidth � 
ItemHeight�� 5   
    5      �� 5      $( "Styles/DataTemplateDictionary.xaml?�6%   %�   &	        &	 4     �� 5   
   .R�5      �  . 6   5      �� 5   
   .R�5      �  6   �� *
rowsInGrid+ 69   �� *
rowsInGrid+5      5      .��5      ]� .+�5   
   ]� $	��Top=�6   .+�5      �� $��1q�6   ._�5      �� -   5        $��ClearFilterControl��5   )   $��Clear��6   6C   5      �� 5      -   5        $��FilterControl��5      $��*coffee filter*��6+   $
��Left=�6B   
�������6�   5      ]� 5    
   $��Bottom=�6   .+�5!      �� -   5        $��ViewTypesComboBox��5!   v   $
��Left=�6    �� SelectedIndex$ 0��6-   @�5"      �� 5#      $��Details��6#   �� 5$      $
��Grid��6#   5&      �� 5'      	 � Maximum$		 200Q�6>   
 � Minimum$
 40Q�6L   $��1q�6Y    � Value 6   �� *
rowsInGrid+ �� Mode$ TwoWay=�6Y   5(      �� 5)   
   -   5        $��
FilterView��5)      $��Disabled=�6%    %� IsVirtualizing .  %� VirtualizationMode$ 	Recycling=�5*   ;    %� 
ScrollUnit$
 Item=�6l    J� 	AllowDrop . #��[  5,      ��5-      �� 5.      .R�5/      �  � ColumnDefinitions 50      �� 51       z� SharedSizeGroup$ AssetIconColSize��6/   .+�53      ]� .+�54      �� $	��Top=�6>   ��6$   �� *
filename+6>   �� 56      ��6    �� *img+57      58      59      5;      5<      5=      