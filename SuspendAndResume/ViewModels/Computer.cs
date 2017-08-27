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
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Windows.Data;
using System.Diagnostics;

namespace SuspendAndResume
{
    public enum ComputerState
    {
        Unknown, Offline, Online
    }

    public class Computer : INotifyPropertyChanged
    {
        private DateTime lastSeen;

        private string name;

        private ComputerState state;

        private IPAddress ipAddress;

        private bool discovered;
        
        public Dictionary<MacAddress, List<IPAddress>> NetworkInterfaces { get; set; }

        public string ID { get; set; }

        public string ServiceUrl { get; set; }

        public IPAddress IPAddress
        {
            get { return ipAddress; }
            set
            {
                if (value != ipAddress)
                {
                    ipAddress = value;
                    NotifyPropertyChanged("IPAddress");
                }
            }
        }

        public DateTime LastSeen
        {
            get { return lastSeen; }
            set
            {
                discovered = true;

                if (value != lastSeen)
                {
                    lastSeen = value;
                    NotifyPropertyChanged("LastSeen");
                }
            }
        }

        public string Name
        {
            get { return name; }
            set
            {
                if (value != name)
                {
                    name = value;
                    NotifyPropertyChanged("Name");
                }
            }
        }

        public ComputerState State
        {
            get { return state; }
            set
            {
                if (value != state)
                {
                    state = value;
                    NotifyPropertyChanged("State");
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public Computer()
        {
            NetworkInterfaces = new Dictionary<MacAddress, List<IPAddress>>();
        }

        public void StartDiscovery()
        {
            discovered = false;
        }

        public void StopDiscovery()
        {
            if (discovered)
            {
                State = ComputerState.Online;
            }
            else
            {
                State = ComputerState.Offline;
            }
        }

        private void NotifyPropertyChanged(String propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;

            if (null != handler)
            {
                handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public void RefreshFrom(Computer computer)
        {
            Debug.Assert(computer.ID == this.ID);

            this.LastSeen = computer.LastSeen;
            this.ServiceUrl = computer.ServiceUrl;
            this.NetworkInterfaces = computer.NetworkInterfaces;
            this.IPAddress = computer.IPAddress;
        }
    }
}
