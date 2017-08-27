using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace SuspendAndResume
{
    public class WorkItemTracker
    {
        private object syncRoot = new Object();

        private int workItems = 1;

        private Action completedCallback;

        public WorkItemTracker(Action completedCallback)
        {
            this.completedCallback = completedCallback;
        }

        private void ThrowIfDone()
        {
            if (workItems == 0)
            {
                throw new InvalidOperationException();
            }
        }
        
        public void BeginTask()
        {
            lock (syncRoot)
            {
                ThrowIfDone();
                workItems++;
            }
        }

        public void EndTask()
        {
            lock (syncRoot)
            {
                ThrowIfDone();
                workItems--;

                if (workItems == 0)
                {
                    completedCallback();
                }
            }
        }
    }
}
