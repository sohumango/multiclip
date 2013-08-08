CXX = g++
TARGET = multiclip
CXXFLAGS = -Wall -O2 -pipe

SRCS = multiclip.cpp
    
ODIR=../objcs
	
objs = $(patsubst %.cpp,$(ODIR)/%.o,$(SRCS))


$(TARGET):things $(objs)
	$(CXX) -o $(ODIR)/$@ $(objs) $(LIBS)
	
things:
	@if [ ! -d "$(ODIR)" ]; then mkdir $(ODIR); fi
	
$(ODIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $(patsubst %.cpp,$(ODIR)/%.o,$<)

    
clean:
	rm -f $(TARGET) $(objs)

new:
	cp ./$(ODIR)/$(TARGET) ../$(TARGET)
	
copy:
	cp ./$(ODIR)/$(TARGET) ../$(TARGET)
