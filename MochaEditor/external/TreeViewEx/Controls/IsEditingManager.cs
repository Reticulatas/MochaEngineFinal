﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Input;

namespace System.Windows.Controls
{
    class IsEditingManager:InputSubscriberBase
    {
        TreeViewEx treeview;
        TreeViewExItem editedItem;

        public IsEditingManager(TreeViewEx treeview)
        {
            this.treeview = treeview;
        }

        internal override void OnMouseDown(Input.MouseButtonEventArgs e)
        {
            base.OnMouseDown(e);
            StopEditing();
        }

        public void StartEditing(TreeViewExItem item)
        {
            StopEditing();
            editedItem = item;
            editedItem.IsEditing = true;
        }

        internal void StopEditing()
        {
            if (editedItem == null) return;

            Keyboard.Focus(editedItem);
            editedItem.IsEditing = false;
            FocusHelper.Focus(editedItem);
            editedItem = null;
        }
    }
}
