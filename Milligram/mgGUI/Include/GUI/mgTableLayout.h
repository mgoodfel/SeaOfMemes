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
#ifndef MGTABLELAYOUT_H
#define MGTABLELAYOUT_H

#include "mgLayoutManager.h"

class mgTableRow;
class mgTableCell;
//class mgFrame;

/*
  An enum of table cell alignment options.
*/
enum  mgTableAlign { 
  mgTableAlignLeft        = 0x1,
  mgTableAlignRight       = 0x2,
  mgTableAlignHCenter     = 0x3,
  mgTableAlignHFill       = 0x4,
  
  mgTableAlignTop         = 0x10,
  mgTableAlignBottom      = 0x20,
  mgTableAlignVCenter     = 0x30,
  mgTableAlignVFill       = 0x40,
};

/*
  An mgLayoutManager which arranges child controls as rows of columns, 
  similar to HTML tables.
*/
class mgTableLayout : public mgLayoutManager
{
public:
  // constructor and destructor
  mgTableLayout(
    mgControl* parent);
    
  virtual ~mgTableLayout();
  
  // set outer frame
  virtual void setFrame(
    const mgFrame* frame);
    
  // add a row
  virtual void newRow();
  
  // end a row
  virtual void endRow();
  
  // add a cell to the table
  virtual void addCell(
    mgControl* child,
    mgTableAlign hAlign,
    mgTableAlign vAlign,
    int rowSpan,
    int colSpan);

  // set frame around current cell
  virtual void setCellFrame(
    const mgFrame* frame);
    
  // set sizing weight for row
  virtual void setRowWeight(
    int weight);

  // set top inset for row
  virtual void setRowTopInset(
    int top);

  // set bottom inset for row
  virtual void setRowBottomInset(
    int bottom);

  // set sizing weight for cell
  virtual void setColWeight(
    int weight);
    
  // set left insets for column
  virtual void setColLeftInset(
    int left);
   
  // set right insets for column
  virtual void setColRightInset(
    int right);
    
  // find a child
  virtual void* findChild(
    mgControl* child);

  // set new child for cell
  virtual void setChild(
    void* voidCell,
    mgControl* child);
        
  // remove all children
  virtual void removeAll();
  
  // remove child from layout
  virtual void removeChild(
    mgControl* child);
  
  virtual void minimumSize(
    mgDimension& size);

  virtual void preferredSize(
    mgDimension& size);

  // return preferred size at width
  virtual BOOL preferredSizeAtWidth(
    int width,
    mgDimension& size);

  virtual void controlResized();

  virtual void paintBackground(
    mgContext* gc);

  virtual void paintForeground(
    mgContext* gc);

protected:
  const mgFrame* m_frame;           // outer frame of table
  
  mgPtrArray m_rows;                // rows of table
  int m_rowCount;                   // final size of grid
  int m_colCount;
    
  mgTableRow* m_row;                // current row
  mgTableCell* m_cell;              // current cell
  int m_colNum;                     // current column number (taking spans into account)

  int m_rowVWeight;                 // vweight for cells in row
  int m_rowTopInset;                // top inset for cells in row
  int m_rowBottomInset;             // bottom inset for cells in row
  mgPtrArray m_colData;             // column defaults
  
  int m_tableMinWidth;
  int m_tableMaxWidth;
  int m_tableMinHeight;
  int m_tableMaxHeight;

  int* m_colUsed;
  int* m_colTargets;

  int* m_minWidths;
  int* m_maxWidths;
  int* m_colWeights;
  int* m_minHeights;
  int* m_maxHeights;
  int* m_rowWeights;

  // find the number of rows an columns
  virtual void getGridSize();
  
  // initialize table layout state
  virtual void initLayout();
  
  // free table layout state
  virtual void termLayout();
  
  // initialize col width ranges from cell width ranges
  virtual void initColWidths();

  // stretch a range of cells to be at least target width
  virtual void stretchCols(
    int targetWidth,             // size we need
    int col,                     // starting column
    int colSpan,                 // count of spanning columns
    BOOL fill);                  // fill target width

  // distribute added column width
  virtual int distribWidth(
    int totalWidth,                // size we need
    int col,                       // starting column
    int colSpan);                  // count of spanning columns

  // get row heights
  virtual void getRowHeights();

  // stretch rows to at least requested height
  virtual void stretchRows(
    int targetHeight,
    int row,
    int rowSpan,
    BOOL fill);

  // distribute added row height
  virtual int distribHeight(
    int totalHeight,                // size we need
    int row,                       // starting column
    int rowSpan,                   // count of spanning columns
    int* targetHeights);           // weights to use

  // return current table width range
  virtual void getTableSize();

  // position cells of table
  virtual void positionChildren(
    int originX,
    int originY);
};

#endif

