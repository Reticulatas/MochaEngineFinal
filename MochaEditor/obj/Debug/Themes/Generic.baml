   M S B A M L   `   `   `  ����F  BMochaEditor, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null9!clr-namespace:MochaEditorControlsMochaEditorControls  5      ?$clr-namespace:MochaEditor.ConvertersMochaEditor.Converters  5      ;"clr-namespace:MochaEditor.ExternalMochaEditor.External  5      \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35` \RibbonControlsLibrary, Version=3.5.41019.1, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e0891    +MochaEditorControls.HierarchyDragController   hierarchyDragController*   $MochaEditorControls.HierarchyControl  �� 
TargetType �� 	Resources9   3MochaEditor.Converters.BooleanToVisibilityConverter  Key �� Property �� Value �� 
TargetType ��Name J� 	AllowDrop �� Orientation d� 	IsChecked	 �� RelativeSource
 �� AncestorType �� Path  BoolToVisConv �� 	Converter  primaryBackgroundBrush �� EdgeMode �� Triggers P� Property P� Value �� 
TargetName ��Name �� FallbackValue �� Triggers �� Binding �� Value �� ElementName ��Name�� 5      H 9http://schemas.microsoft.com/winfx/2006/xaml/presentation      :x,http://schemas.microsoft.com/winfx/2006/xaml     5      +local!clr-namespace:MochaEditorControls  5      -conv$clr-namespace:MochaEditor.Converters  5      *ext"clr-namespace:MochaEditor.External  5      %!  &	        '       7  7     5	      6$   �� 5      "   6    5   
   �� 5        5      BoolToVisConv 64   5      .�5   
   ��  � �� 5      �� 5      "  6"   .R�5      �� #��z��6   #��z��5      #��z��5      $��8,0,0,0q�6Q   ._�5      �� .+�5      �� $ HierarchySeparator��69    . $��1��6U   �� 5      $ 
Horizontal=�6)   .+�5   "   d� $��ExpandedToggleButton��6/   $
 TrueY�6K   y�6\   �� 	 �� "
  $ IsFolder�8   ��5   &   �� 5   *   "  d�60   .�5   .   ��  � |� .  6J   �� 5    .    � �� 5!   2   �� 5"   6   " d�6F   .R�5#   :   � $��14��6?   $��14��6J   .+�5$   >   �� #���  6H   �� 5%   >   $��ExpandBorder��6E   $ Aliased=�6Y   
���   �$��2q�6�   ._�5&   B   F� $��
ExpandPath��6h   $ Aliased=�6G   
���   �$��0q�6�   $��2��6�   +����@KL @B �@KL @T� �@B @KL �@T� @KL 	6�   5'   ?   5(   ;    5)   :   P� 5*   >    �@d�	IsChecked$
 TrueY�6[   . �5+   B   �� $ 
ExpandPath��6Y    � �� ��@B @KL �@T� @KL 	6q   5,   ?   P� 5-   >    � {� .  . �5.   B   �� $ ExpandBorder��6I    � ��
 �����6z   �� 5/   B   $ 
ExpandPath��6I    � ��
 �����6s   �� 50   B   $ 
ExpandPath��6Y    � �� �	6q   51   ?   53   7   55   /   56   +   58   #   �� 59   "   $ HierarchyText��6l    . ��6,   �� $
 Name�$
 Root����5:   &   �� 5;   *   "  ��60    5<   .   �� 5=   2    6>   �� 	 �� "
  $ 
IsSelected� True6�   .a�5>   6   ��  � ��
 � � �6S   5?   3   �� 5@   2    6>   �� 	 �� "
  $ 
IsSelected�	 False6�   .a�5A   6   ��  � ��
 ���� 6S   5B   3   5D   +   5F   #   5G      �� 5H      $��ChildrenExpander��6)   y�6A   �� *	IsChecked+$ ExpandedToggleButton��8  .+�5I   "   �� $ HierarchyBorder��6�   $��1q�6)   
������� . ��6P   �� 	 �� "
  $ 
PanelColor�._�5J   &   �� 5K   #   5L      5M      5N      5O      5Q      5R      5S      