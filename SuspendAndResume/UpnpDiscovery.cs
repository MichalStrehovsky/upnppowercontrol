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
using System.Net.Sockets;
using System.Text;
using System.Diagnostics;
using System.Collections.Generic;
using System.IO;

namespace SuspendAndResume
{
    public class ResponseReceivedEventArgs : EventArgs
    {
        public Dictionary<string, string> Headers
        {
            get; private set; 
        }

        public IPAddress IPAddress
        {
            get;
            private set;
        }

        public ResponseReceivedEventArgs(Dictionary<string, string> headers, IPAddress ipAddress)
        {
            Headers = headers;
            IPAddress = ipAddress;
        }
    }

    public class SsdpFinder
    {
        private string searchTarget;

        private int maximumWait;

        Timer timer;

        public SsdpFinder(string searchTarget = "ssdp:all", int maximumWait = 1)
        {
            this.searchTarget = searchTarget;
            this.maximumWait = maximumWait;
        }

        public event EventHandler<ResponseReceivedEventArgs> ResponseReceived;

        public event EventHandler DiscoveryCompleted;

        protected virtual void OnResponseReceived(ResponseReceivedEventArgs e)
        {
            if (ResponseReceived != null)
            {
                ResponseReceived(this, e);
            }
        }

        protected virtual void OnDiscoveryCompleted()
        {
            if (DiscoveryCompleted != null)
            {
                DiscoveryCompleted(this, EventArgs.Empty);
            }
        }

        private static Dictionary<string, string> ParseSsdpResponse(string response)
        {
            StringReader reader = new StringReader(response);

            string line = reader.ReadLine();
            if (line != "HTTP/1.1 200 OK")
            {
                return null;
            }

            Dictionary<string, string> result = new Dictionary<string, string>(StringComparer.InvariantCultureIgnoreCase);

            line = reader.ReadLine();
            while (line != null)
            {
                if (line != String.Empty)
                {
                    int colon = line.IndexOf(':');
                    if (colon < 1)
                    {
                        return null;
                    }
                    string name = line.Substring(0, colon).Trim();

                    string value = line.Substring(colon + 1).Trim();
                    if (string.IsNullOrEmpty(name))
                    {
                        return null;
                    }

                    result[name] = value;
                }

                line = reader.ReadLine();
            }

            return result;
        }

        public void FindAsync()
        {
            const string multicastIP = "239.255.255.250";
            const int multicastPort = 1900;
            const int unicastPort = 1901;
            const int MaxResultSize = 8000;

            string find = "M-SEARCH * HTTP/1.1\r\n" +
               "HOST: 239.255.255.250:1900\r\n" +
               "MAN: \"ssdp:discover\"\r\n" +
               "MX: " + maximumWait.ToString() + "\r\n" +
               "ST: " + searchTarget + "\r\n" +
               "\r\n";

            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            byte[] MulticastData = Encoding.UTF8.GetBytes(find);
            socket.SendBufferSize = MulticastData.Length;
            SocketAsyncEventArgs sendEvent = new SocketAsyncEventArgs();
            sendEvent.RemoteEndPoint = new IPEndPoint(IPAddress.Parse(multicastIP), multicastPort);
            sendEvent.SetBuffer(MulticastData, 0, MulticastData.Length);
            sendEvent.Completed += new EventHandler<SocketAsyncEventArgs>((sender, e) =>
            {
                if (e.SocketError == SocketError.OperationAborted)
                {
                }
                else if (e.SocketError != SocketError.Success)
                {
                    Debug.WriteLine("Socket error {0}", e.SocketError);
                }
                else
                {
                    if (e.LastOperation == SocketAsyncOperation.SendTo)
                    {
                        // When the initial multicast is done, get ready to receive responses
                        e.RemoteEndPoint = new IPEndPoint(IPAddress.Any, unicastPort);
                        socket.ReceiveBufferSize = MaxResultSize;
                        byte[] receiveBuffer = new byte[MaxResultSize];
                        e.SetBuffer(receiveBuffer, 0, MaxResultSize);
                        socket.ReceiveFromAsync(e);
                    }
                    else if (e.LastOperation == SocketAsyncOperation.ReceiveFrom)
                    {
                        // Got a response, so decode it
                        var result = Encoding.UTF8.GetString(e.Buffer, 0, e.BytesTransferred);
                        var headers = ParseSsdpResponse(result);
                        if (headers != null)
                        {
                            Debug.WriteLine(result);
                            OnResponseReceived(new ResponseReceivedEventArgs(headers, (e.RemoteEndPoint as IPEndPoint).Address));
                        }
                        else
                        {
                            Debug.WriteLine("INVALID SEARCH RESPONSE");
                        }

                        // And kick off another read
                        try
                        {
                            socket.ReceiveFromAsync(e);
                        }
                        catch (ObjectDisposedException)
                        {
                        }
                    }
                }
            });

            // Kick off the initial Send
            socket.SendToAsync(sendEvent);

            // Set a one-shot timer for double the Search time, to be sure we are done before we stop everything
            TimerCallback cb = new TimerCallback((state) =>
            {
                socket.Close();

                timer.Dispose();
                timer = null;

                // BUGBUG: we can still be in sendEvent.Completed on another thread
                OnDiscoveryCompleted();
            });
            timer = new Timer(cb, timer, (maximumWait * 2) * 1000, Timeout.Infinite);
       }
    }
}
