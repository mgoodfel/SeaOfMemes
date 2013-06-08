/*
  Copyright (C) 1995-2013 by Michael J. Goodfellow

  This source code is distributed for free and may be modified, redistributed, and
  incorporated in other projects (commercial, non-commercial and open-source)
  without restriction.  No attribution to the author is required.  There is
  no requirement to make the source code available (no share-alike required.)

  This source code is distributed "AS IS", with no warranty expressed or implied.
  The user assumes all risks related to quality, accuracy and fitness of use.

  Except where noted, this source code is the sole work of the author, but it has 
  not been checked for any intellectual property infringements such as copyrights, 
  trademarks or patents.  The user assumes all legal risks.  The original version 
  may be found at "http://www.sea-of-memes.com".  The author is not responsible 
  for subsequent alterations.

  Retain this copyright notice and add your own copyrights and revisions above
  this notice.
*/
#ifndef MGRESOURCE_H
#define MGRESOURCE_H

/*
  The parent of all persistent resources.  A table is kept in the
  mgSurface instance, mapping resource specification (ex: font name)
  to a resource instance.  Fonts can be created repeated and the
  same instance will be returned.  The application should never delete a 
  resource -- they are deleted when the surface instance is destroyed.
*/

class mgResource
{
public:
  mgString m_key;
  void* m_handle;       // platform resource handle

  mgResource()
  {
    m_handle = NULL;
  }

  // destructor
  virtual ~mgResource()
  {}
};

#endif