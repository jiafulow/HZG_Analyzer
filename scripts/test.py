#! /usr/bin/env python3

class Thing:
  def __init__(self, **kwargs):
    for arg in kwargs:
      self.attrs[arg] = kwargs[arg]

thing1 = Thing(color='red')

