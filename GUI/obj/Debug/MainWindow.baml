   M S B A M L   `   `   `  ����>  :GUI, Version=1.0.0.0, Culture=neutral, PublicKeyToken=nullclr-namespace:GUIGUI  5   	       GUI.MainWindow   5   	   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5   	   R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	   @d2http://schemas.microsoft.com/expression/blend/2008     5   	   Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5   	   #localclr-namespace:GUI     5   	   -   5          � 
ResizeMode$  NoResize=�5   	    � Title$  Client GUI Demo Req #4,#6 ��5	   	   $	��650��6,   $	��600��69   .��5      �  � ColumnDefinitions 5      �� 5   
   $��8* �6    � RowDefinitions 5      �� 5   
   $��* �6   �� 5   
   $��25 �6   .+�5      �� $��0��6   $��tahoma)�6%   $
��Bold"�69   $��16(�6K   ���#ddd.@�5      �� $��Find Libraries��6   $
��10 5q�6(   .��5   
   ]� .+�5      ��  �� Orientation$ 
Horizontal=�6   $	��Top=�60   .+�5      �� .��6   Path:6   �� 5      $��20��6   6!   �� 5       -   5         ��Name$ CurrPath��5       $��Center=�6C   $��Medium"�6^   $��12(�6r   6�   5!      � 5"       5#      �� 5$      $��* �6   6+   �� 5%      $��5 �6   6+   �� 5&      $��* �6   6+   .+�5(      �� -   5         ��Name$
 Dirs��5(   #   $��0��6   $��5q�6J   $��Medium"�6U   $��12(�6i   6�   �� 5)      $��1��6   $��Stretch=�6(   ���#9996Z   �� 5*      -   5        $ Files��5*   #   $��2��6   $��5q�6K   $��Medium"�6V   $��12(�6j    �� SelectionMode$ Single=�6x   6�   5+      5,      5-   	   �� 5.   
   $��Send Request��6   $
��10 5q�6(   .��5/      ]� .+�50      �  51      �� 52      $��5* �6#   60   �� 53      $��5* �6#   60    55      �� 56      $��1* �6    6.   �� 57      $��9* �6    6.   .+�59      �� $��0��6   $��2��6+   .+�5:      �� $
��Left=�6   $��Request Name:��6.   �� 6H   -   5         ��Name$ tq_name��5:   P   $��0��6b   $��1��6s   6�   �� 5;      $
��Left=�6   $��Request Author:��6.   �� 6J   -   5        $ 	tq_author��5;   R   $��0��6e   $��1��6v   6�   �� 5<      $��0��6   $��1��6/   $��5��6<   $��Stretch=�6G   �� 5=      -   5        	 ��Name$	 SendRequest��5=   6   $��0��6   $��1��6)   $
��Send��6K   6Z   �� 5>      $��0��6   $��1��6/   $��5��6<   $��Stretch=�6G   �� 5?      -   5        $	 ClearRequest��5?   6   $��0��6   $��1��6)   $��Clear��6L   6\   5@      �� 5A      $��1��6   $��2��6+   .+�5B      �� $��1��6   $��0��6*   .��67   Selected Libraries:6M   �� 5C      -	   5        $ 	show_dlls��5C   9   $��1��6   $��1��6,   $��5q�6L   $	��250��6W   $��Medium"�6d   $��12(�6x   ���#FFF6�   5D      5E      5F      5G      �� 6   $��Test Results��6   $
��10 5q�62   .��5H   
   ]� .+�5I      �� $ 
Horizontal=�6   $	��Top=�60   .+�5J      �� .��6   Test result messages:6,   �� 5K      $��20��6   6!   5L      � 5M       5N      �� 5O      $��* �6   6+   .+�5Q      �� -
   5        $ result��5Q   #   $��0��6   $
��Auto��63   $
��Auto��6A   $��5q�6g   $��Medium"�6r   $��12(�6�   6�   5R      5S      5T   	   5U      �� 5V      $��1��6   ���#ddd._�5W   
   �� -   5        $ status��5W      $��0��6$   $��
Status Bar��64   $��25��6F   
 �� Padding$
 5q�6R   $��tahoma)�6^   6t   5X   	   5Y      5Z      