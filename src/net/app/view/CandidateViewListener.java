package net.app.view;

public abstract interface CandidateViewListener
{
 // public abstract void onClickChoice(int paramInt);

  public abstract void onToBottomGesture();

  public abstract void onToLeftGesture();

  public abstract void onToRightGesture();

  public abstract void onToTopGesture();
}