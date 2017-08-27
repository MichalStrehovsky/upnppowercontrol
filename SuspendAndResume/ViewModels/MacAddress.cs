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
using System.Text;
using System.Globalization;
using System.Runtime.Serialization;

namespace SuspendAndResume
{
    [DataContract]
    public sealed class MacAddress : IEquatable<MacAddress>
    {
        [DataMember]
        private byte[] address;

        private MacAddress()
        {
        }

        public static MacAddress FromString(string address)
        {
            if (String.IsNullOrEmpty(address))
            {
                throw new ArgumentNullException("address");
            }

            MacAddress result = new MacAddress();

            string[] components = address.Split('-');

            result.address = new byte[components.Length];

            try
            {
                for (int i = 0; i < components.Length; i++)
                {
                    result.address[i] = Byte.Parse(components[i], NumberStyles.HexNumber);
                }
            }
            catch (Exception ex)
            {
                if (ex is ArgumentException ||
                    ex is FormatException ||
                    ex is OverflowException)
                {
                    throw new ArgumentException("Address has invalid format");
                }

                throw;
            }

            return result;
        }

        public bool Equals(MacAddress other)
        {
            if (other == null)
            {
                return false;
            }

            int length = address.Length;
            if (length != other.address.Length)
            {
                return false;
            }

            for (int i = 0; i < length; i++)
            {
                if (address[i] != other.address[i]) return false;
            }

            return true;
        }

        public override bool Equals(object obj)
        {
            if ((obj == null) || obj.GetType() != GetType())
            {
                return false;
            }

            return Equals(obj as MacAddress);
        }

        public override int GetHashCode()
        {
            int hash = 5381;

            for (int i = 0; i < address.Length; i++)
            {
                hash = ((hash << 5) + hash) + address[i];
            }

            return hash;
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder((address.Length * 3) - 1);

            for (int i = 0; i < address.Length; i++)
            {
                sb.AppendFormat(i == 0 ? "{0:X2}" : "-{0:X2}", address[i]);
            }

            return sb.ToString();
        }
    }
}
