   M S B A M L   `   `   `  ����  MochaEditor$    MochaEditor.GameObjectViewItem   5      \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35` \RibbonControlsLibrary, Version=3.5.41019.1, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35H 9http://schemas.microsoft.com/winfx/2006/xaml/presentation      5      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5      Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5      @d2http://schemas.microsoft.com/expression/blend/2008     5        � 	Resources  5	       �� MergedDictionaries �� Source�� 5
   
    5      �� 5      $ Styles/Styles.xaml?�6%   %    5      .��5      �� -   5         �� Orientation$ Vertical=�5      '  !System.Windows.VisualStateManager   VisualStateGroups 5   
   %  System.Windows.VisualStateGroup  5      -   5        	Expanding��   Transitions 5      %  System.Windows.VisualTransition  5         From$ 	Collapsed��6'   	   To$ Expanded��68      GeneratedDuration$ 0:0:1D�6F   	   
Storyboard.	 5      �� .��5      T� 
 g� Duration$
 0:0:0D�6<    �� 
TargetName$ Expanded_Content��6M    �� TargetProperty$ (UIElement.Visibility)�6v   ..�5   "   i�  R� KeyTime$ 0:0:0��69     System.Windows.Visibility   Visible R� Value# Z 6I   5      5      5         States. 5         System.Windows.VisualState  -   5        	Collapsed��   
Storyboard. 5      �� .��5       T� $
 0:0:0D�68   $ Expanded_Content��6I   $ (UIElement.Visibility)�6r   ..�5!      i� $ 0:0:0��65      	Collapsed# Z 6E   5"      5#      5$        5%      -   5        Expanded��. 5&      �� .��5'      T� $
 0:0:0D�68   $ Expanded_Content��6I   $ (UIElement.Visibility)�6r   ..�5(      i� $ 0:0:0��65   # Z 6E   5)      5*      5+      5,      .+�5/   
   �� $ 
Horizontal=�6   $��20��6.   .+�50      �� $��20��6/   $	��100��6;      GrowingTextStyle#��[  6J   $��13(�6�   
���������6   �� *mName+52      53      �� 54   
   -   5        $��Expanded_Content��54   ,   $ Vertical=�6   $y�	Collapsed=�6D   .+�55      �� $��^��6   $��Center=�6"   $��Center=�6?   $��0q�56      
�������
���OO/�
�������.��57      �� $ Vertical=�6   .+�58      �� -   5        $��DeleteButton��58      $��delete��61   6B   �� 59      -   5        $��DuplicateButton��59   1   $��	duplicate��6   6H   �� 5:      -   5        $��NoParentButton��5:   .   $��detach��6   6D   �� 5;      -	   5        $��MakePrefabButton��5;   3   $��make prefab��6   6K   5<      5=      5>      5?      5@      