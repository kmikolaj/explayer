#ifndef DPMS_H
#define DPMS_H

class DPMS
{
public:
	DPMS();
	void Enable();
	void Disable();
	void Restore();
private:
	void Store();
	bool State();
	bool state;
};

#endif // DPMS_H
