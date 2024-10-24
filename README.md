# Openflow switch connection manager and message handler

Openflow Connection Manager manages connection attachments - and filter all
messages using Version specific parsers. Connection Manager and Parsers - are
designed using decorator pattern so that any new Openflow Spec can be
implemented without much affecting the previous spec. Currently OF1.3 spec supported.

# Features

10,000 feet view: 

- Object-Oriented design and patterns were followed.
- Each Configuration is attached to Abstract Connections 
  which then gets managed within OfConnManager class.
- Abstract Connection is then extended using Generic Connection that make use
  of either SslClient or TcpClient
- Openflow Connection Manager is a property of Logical Switch


# Designer and Author

Syed Mamun Raihan


# License
Propritery for deployment. Free to evaluate. Customization is needed for actual use.
