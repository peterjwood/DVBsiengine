## DVBsiengine
A C++ set of classes and utilities for using DVB SI data

### Background
This project started many years ago when I was working in the DVB industry as my own analytical tool for DVB systems when I found that the tools available did not do what I wanted

The project was also used as a way to teach myself C++ and as such has evolved over the years.

It initially started out as a Windows Visual C++ project in 2002 and has since been rewritten several times to work on Linux commandline and also with a QT back end.  In it's current form it is a Linux command line system using fifo based systems for filtering of PIDS and tables from Multi Program Transport Stream files that are in 188 or 204 byte packet format.  There are apps for converting these to section files and then readinng the information from the sections to present the tables in serveral formats

The system itself has a reader and writer class system to allow input and output from/to different sources/sinks with a text output that can be expressed as JSON or a simple tree like representation

There are functions for manipulating DSMCC data and object carousels as well as more standard tables such as EIT/PAT/CAT/SDT
