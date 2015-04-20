package net.app.view;
import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.FrameLayout.LayoutParams;
import android.widget.HorizontalScrollView;

public class RightAlignedHorizontalScrollView
  extends HorizontalScrollView
{
  private boolean a = false;
  private boolean b = false;
  
  public RightAlignedHorizontalScrollView(Context paramContext)
  {
    super(paramContext);
  }
  
  public RightAlignedHorizontalScrollView(Context paramContext, AttributeSet paramAttributeSet)
  {
    super(paramContext, paramAttributeSet);
  }
  
  public RightAlignedHorizontalScrollView(Context paramContext, AttributeSet paramAttributeSet, int paramInt)
  {
    super(paramContext, paramAttributeSet, paramInt);
  }
  
  private int getScrollRange()
  {
    int i = getChildCount();
    int j = 0;
    if (i > 0) {
      j = Math.max(0, getChildAt(0).getWidth() - (getWidth() - getPaddingLeft() - getPaddingRight()));
    }
    return j;
  }
  
  public void computeScroll()
  {
    super.computeScroll();
  }
  
  public View getChildView()
  {
    return getChildAt(0);
  }
  
  protected void onLayout(boolean paramBoolean, int paramInt1, int paramInt2, int paramInt3, int paramInt4)
  {
    this.b = false;
    int i = getChildView().getWidth();
    super.onLayout(paramBoolean, paramInt1, paramInt2, paramInt3, paramInt4);
    int j = getChildView().getWidth() - i;
    View localView = getChildView();
    FrameLayout.LayoutParams localLayoutParams = (FrameLayout.LayoutParams)localView.getLayoutParams();
    int k = 0x7 & localLayoutParams.gravity;
    int m = 0x70 & localLayoutParams.gravity;
    if (k == 5) {
      if (getScrollRange() > 0)
      {
        this.a = true;
        localLayoutParams.gravity = (m | 0x3);
        localView.setLayoutParams(localLayoutParams);
        super.onLayout(paramBoolean, paramInt1, paramInt2, paramInt3, paramInt4);
      }
    }
    for (;;)
    {
      if ((this.a) && (j > 0))
      {
        scrollBy(j, 0);
        this.b = true;
      }
      //return;
      if ((this.a) && (getScrollRange() == 0))
      {
        this.a = false;
        localLayoutParams.gravity = (m | 0x5);
        localView.setLayoutParams(localLayoutParams);
        super.onLayout(paramBoolean, paramInt1, paramInt2, paramInt3, paramInt4);
      }
    }
  }
  
  public void scrollTo(int paramInt1, int paramInt2)
  {
    if (this.b) {
      return;
    }
    super.scrollTo(paramInt1, paramInt2);
  }
}

