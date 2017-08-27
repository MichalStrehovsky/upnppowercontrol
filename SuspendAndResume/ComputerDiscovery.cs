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
using System.Threading;
using System.Diagnostics;
using System.Xml.Linq;
using System.Linq;

namespace SuspendAndResume
{
    public class ComputerDiscoveredEventArgs : EventArgs
    {
        public Computer Computer { get; set; }

        public ComputerDiscoveredEventArgs(Computer computer)
        {
            Computer = computer;
        }
    }

    public class ComputerDiscovery
    {
        private SsdpFinder ssdpFinder;

        private WorkItemTracker workTracker;

        public event EventHandler<ComputerDiscoveredEventArgs> ComputerDiscovered;

        public event EventHandler DiscoveryCompleted;

        private const string UpnpDeviceNamespace = "urn:schemas-upnp-org:device-1-0";

        public ComputerDiscovery()
        {
            ssdpFinder = new SsdpFinder("urn:migeel-sk:device:PowerStateDevice:1");

            ssdpFinder.ResponseReceived += new EventHandler<ResponseReceivedEventArgs>(SsdpFinder_ResponseReceived);
            ssdpFinder.DiscoveryCompleted += new EventHandler(SsdpFinder_DiscoveryCompleted);
        }

        private void OnDiscoveryCompleted()
        {
            if (DiscoveryCompleted != null)
            {
                DiscoveryCompleted(this, EventArgs.Empty);
            }

            workTracker = null;
        }

        private void OnComputerDiscovered(ComputerDiscoveredEventArgs e)
        {
            if (ComputerDiscovered != null)
            {
                ComputerDiscovered(this, e);
            }
        }

        private void SsdpFinder_DiscoveryCompleted(object sender, EventArgs e)
        {
            Debug.WriteLine("End find");
            workTracker.EndTask();
        }

        private void SsdpFinder_ResponseReceived(object sender, ResponseReceivedEventArgs e)
        {
            string locationUriString = e.Headers["location"];

            if (String.IsNullOrEmpty(locationUriString))
            {
                Debug.WriteLine("No location in headers");
                return;
            }

            Uri location = new Uri(locationUriString);

            Debug.WriteLine("Begin track {0}", location);
            workTracker.BeginTask();

            var wc = new WebClient();
            wc.DownloadStringCompleted += new DownloadStringCompletedEventHandler((o, args) => {
                if (args.Error == null)
                {
                    var doc = XElement.Parse(args.Result);
                    if (doc.GetDefaultNamespace() == UpnpDeviceNamespace)
                    {
                        var computer = new Computer();
                        computer.IPAddress = e.IPAddress;
                        computer.LastSeen = DateTime.Now;
                        computer.Name = (string)doc.Descendants(XName.Get("friendlyName", UpnpDeviceNamespace)).FirstOrDefault();
                        computer.State = ComputerState.Online;

                        OnComputerDiscovered(new ComputerDiscoveredEventArgs(computer));
                    }
                }

                Debug.WriteLine("End track {0}", location);
                workTracker.EndTask();
            });

            wc.DownloadStringAsync(location);
        }

        public void FindAsync()
        {
            workTracker = new WorkItemTracker(OnDiscoveryCompleted);
            ssdpFinder.FindAsync();
        }
    }
}
