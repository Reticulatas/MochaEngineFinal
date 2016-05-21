using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;

public static class ListExtension
{
    public static void BubbleSort(this IList o)
    {
        for (int i = o.Count - 1; i >= 0; i--)
        {
            for (int j = 1; j <= i; j++)
            {
                object o1 = o[j - 1];
                object o2 = o[j];
                if (((IComparable)o1).CompareTo(o2) > 0)
                {
                    o.Remove(o1);
                    o.Insert(j, o1);
                }
            }
        }
    }
    public static void BubbleSortGameObject(this IList o)
    {
        for (int i = o.Count - 1; i >= 0; i--)
        {
            for (int j = 1; j <= i; j++)
            {
                object o1 = o[j - 1];
                object o2 = o[j];
                if (((GameObject)o1).mName.CompareTo((o2 as GameObject).mName) > 0)
                {
                    o.Remove(o1);
                    o.Insert(j, o1);
                }
            }
        }
    }

    public static void BubbleSortGameObjectLevenstein(this IList o, string input)
    {
        for (int i = o.Count - 1; i >= 0; i--)
        {
            for (int j = 1; j <= i; j++)
            {
                object o1 = o[j - 1];
                object o2 = o[j];
                if (WPFHelper.LevenshteinDistance.Compute(((GameObject)o1).mName, input) > WPFHelper.LevenshteinDistance.Compute(((GameObject)o2).mName, input))
                {
                    o.Remove(o1);
                    o.Insert(j, o1);
                }
            }
        }
    }

}