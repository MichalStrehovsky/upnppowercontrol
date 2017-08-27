using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;
using System.Windows.Data;
using System.Globalization;

namespace SuspendAndResume
{
    public class ComputerStatusConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var state = (ComputerState)value.GetType().GetProperty("State").GetValue(value, null);

            if (targetType == typeof(string))
            {
                if (state == ComputerState.Online)
                {
                    return "Online";
                }
                else if (state == ComputerState.Unknown)
                {
                    return "Unknown";
                }
                {
                    var lastSeen = (DateTime)value.GetType().GetProperty("LastSeen").GetValue(value, null);
                    return String.Format("Last seen: {0:g}", lastSeen);
                }
            }

            if (targetType == typeof(Style))
            {
                return state == ComputerState.Online ?
                    Application.Current.Resources["PhoneTextAccentStyle"] :
                    Application.Current.Resources["PhoneTextSubtleStyle"];
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }

    public partial class MainPage : PhoneApplicationPage
    {
        // Constructor
        public MainPage()
        {
            InitializeComponent();

            // Set the data context of the listbox control to the sample data
            DataContext = App.ViewModel;
            this.Loaded += new RoutedEventHandler(MainPage_Loaded);
        }

        // Load data for the ViewModel Items
        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            ComputerDiscovery cd = new ComputerDiscovery();

            cd.ComputerDiscovered += new EventHandler<ComputerDiscoveredEventArgs>((o, args) =>
            {
                Dispatcher.BeginInvoke(() =>
                    {
                        App.ViewModel.Computers.Add(args.Computer);
                    });
            });

            cd.DiscoveryCompleted += new EventHandler((o, args) =>
            {
                Dispatcher.BeginInvoke(() =>
                    {
                        progressBar.Visibility = Visibility.Collapsed;
                    });
            });

            progressBar.Visibility = Visibility.Visible;

            cd.FindAsync();

            /*SsdpFinder.FindAsync("urn:migeel-sk:device:PowerStateDevice:1", 4, delegate(Dictionary<string, string> x)
            {
                Dispatcher.BeginInvoke(() => App.ViewModel.Computers.Add(new Computer() { Name = x["Location"] }));
            });*/
            
        }
    }
}