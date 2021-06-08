#pragma once
// Updated 2017-10-21
// - use only floats
class MS20Filter
{
private:
	float k2, k1, T;
	float C1, C2, R1, R2;
	float xm1, xm2, ym1, ym2;
	float Ax, Bx, Cx, Ay, By, Cy;
	float iCy, Cx0, tempTerm1;
	float cutoff, integratedCutoff, resonance, cutmod, pC1, pR1;
	int sampleIdx, SCurve;
	void update();
	void updateConstants();
public:
	MS20Filter(int samplfreq);
	~MS20Filter();
	float calculate(float input);
	void setCutoff(float v);
	void setResonance(float v);
	void setModulation(float v);
	void setSCurve(int v);
	void setComponentValues(float c, float r);
	void reset();
};