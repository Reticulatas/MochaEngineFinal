using System;
using System.Windows;

public class LogEntry 
{
    String _text;

    public String text
    {
        get { return _text; }
        set
        {
            _text = value;
            if (_text.Length > 1 && _text[0] == '[')
            {
                //parse out the time
                String time = _text.Substring(0,text.IndexOf(']'));
                _text = text.Substring(text.IndexOf(']') + 1);
                try   {
                    time = time.Remove(0, 1);
                    string[] times = time.Split(':');
                    if (times.Length == 3)
                    {
                        Time = new DateTime(DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day,
                        Convert.ToInt32(times[0]), Convert.ToInt32(times[1]), Convert.ToInt32(times[2]));
                    }
                    else
                        throw new FormatException("Parse failed, not enough elements in time string");
                }
                catch (FormatException e)
                {
                    Console.WriteLine("Could not parse time because: " + e.Message);
                }
            }
            _text.Trim();
        }
    }

    public DateTime Time
    {
        get;
        set;
    }

    public LogEntry()
    {
        _text = "[blank]";
        Time = DateTime.Now;
    }

    public LogEntry(String s) 
    {
        _text = s;
        Time = DateTime.Now;
    }

}

public class Logger
{
    public static void Log(String s)
    {
        Console.WriteLine(s);
    }
}