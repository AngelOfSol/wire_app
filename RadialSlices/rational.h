#pragma once
#include "math.h"
#include <string>
#include <boost\regex.hpp>

#define DEBUG

template <class IntType>
class Rational
{
public:
	Rational(IntType numerator, IntType denominator):numerator_(numerator), denominator_(denominator)
	{
		this->rationalize();
#ifdef DEBUG
		this->value = (long double)this->numerator_ / (long double) this->denominator_;
#endif
	};
	template <class OldType>
	Rational(Rational<OldType> value):numerator_((IntType)value.numerator()), denominator_((IntType)value.denominator())
	{
#ifdef DEBUG
		this->value = (long double)this->numerator_ / (long double) this->denominator_;
#endif
	}
	Rational():numerator_(0), denominator_(1)
	{
#ifdef DEBUG
		this->value = (long double)this->numerator_ / (long double) this->denominator_;
#endif
	}
	template<class FloatType>
	Rational(FloatType number):numerator_(0), denominator_(1)
	{
		this->assign<FloatType>(number);
		this->rationalize();
#ifdef DEBUG
		this->value = (long double)this->numerator_ / (long double) this->denominator_;
#endif
	};
	
	Rational& operator =(const IntType& rhs)
	{
		this->numerator_ = rhs;
		this->denominator_ = 1;
#ifdef DEBUG
		this->value = (long double)this->numerator_ / (long double) this->denominator_;
#endif
		return *this;
	};
	template<class FloatType>
	Rational& operator =(const FloatType& rhs)
	{
		this->assign(rhs);
#ifdef DEBUG
		this->value = (long double)this->numerator_ / (long double) this->denominator_;
#endif
		return *this;
	};

	Rational operator -() const
	{
		return Rational(-this->numerator_, this->denominator_);
	};

	Rational operator +(const Rational& rhs) const
	{
		return Rational(this->numerator_ * rhs.denominator_ + this->denominator_ * rhs.numerator_, this->denominator_ * rhs.denominator_);
	};

	Rational operator -(const Rational& rhs) const
	{
		return (*this) + (-rhs);
	}
	
	Rational operator *(const Rational& rhs) const
	{
		return Rational(this->numerator_ * rhs.numerator_, this->denominator_ * rhs.denominator_);
	}
	Rational operator /(const Rational& rhs) const
	{
		return Rational(this->numerator_ * rhs.denominator_, this->denominator_ * rhs.numerator_);
	}

	Rational& operator +=(const Rational& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	Rational& operator -=(const Rational& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	Rational& operator *=(const Rational& rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	Rational& operator /=(const Rational& rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	bool operator ==(const Rational& rhs) const
	{
		return this->numerator_ == rhs.numerator_ && this->denominator_ == rhs.denominator_;
	}

	bool operator !=(const Rational& rhs) const
	{
		return *this == rhs;
	}

	bool operator <(const Rational& rhs) const
	{
		return (this->numerator_ * rhs.denominator_) < (this->denominator_ * rhs.numerator_);
	}
	bool operator <=(const Rational& rhs) const
	{
		return *this < rhs || *this == rhs;
	}
	bool operator >(const Rational& rhs) const
	{
		return !(*this <= rhs);
	}
	bool operator >=(const Rational& rhs) const
	{
		return !(*this < rhs);
	}
	


	template <class FloatType>
	bool operator <(const FloatType& rhs) const
	{
		return this->numerator_ < rhs * this->denominator_;
	}
	template <class FloatType>
	bool operator <=(const FloatType& rhs) const
	{
		return this->numerator_ <= rhs * this->denominator_;
	}
	template <class FloatType>
	bool operator >(const FloatType& rhs) const
	{
		return this->numerator_ > rhs * this->denominator_;
	}
	template <class FloatType>
	bool operator >=(const FloatType& rhs) const
	{
		return this->numerator_ >= rhs * this->denominator_;
	}
	template <class FloatType>
	bool operator ==(const FloatType& rhs) const
	{
		return this->numerator_ == rhs * this->denominator_;
	}
	template <class FloatType>
	bool operator !=(const FloatType& rhs) const
	{
		return this->numerator_ != rhs * this->denominator_;
	}

	IntType as() const
	{
		return this->numerator_ / this->denominator_;
	}
	template <class FloatType>
	operator const FloatType() const
	{
		return (FloatType)this->numerator_ / (FloatType)this->denominator_;
	}
	Rational rabs() const
	{
		return Rational(sign(this->numerator_) * this->numerator_, this->denominator_);
	}
	Rational rsqrt() const
	{
		Rational ret;
		ret.assign<double>(std::sqrt((double)*this));
		return ret;
	}
	Rational rsign() const
	{
		return Rational(sign(this->numerator_), 1);
	}
	
	const IntType& denominator() const { return this->denominator_; };
	const IntType& numerator() const { return this->numerator_; };
	
	static boost::regex numberParse;

private:
	template <typename FloatType>
	void assign(const FloatType& rhs)
	{
		int denominator = 1;
		FloatType numerator = rhs;
		int counter = 0;
		this->numerator_ = (IntType)std::floor(numerator * pow(6, 10));
		this->denominator_ = (IntType)pow(6, 10);
		this->rationalize();
	}
	void rationalize()
	{
		this->numerator_ *= sign(this->denominator_);
		this->denominator_ *= sign(this->denominator_);
		IntType divisor = gcd(this->numerator_ * sign(this->numerator_), this->denominator_);
		divisor = divisor ? divisor : 1;
		this->numerator_ /= divisor;
		this->denominator_ /= divisor;

		IntType maxDenom = 1000000;

		if (this->denominator_ > maxDenom)
		{
			this->numerator_ = this->numerator_ * maxDenom / this->denominator_;
			this->denominator_ = maxDenom;
		}
		
	};
	IntType numerator_;
	IntType denominator_;
#ifdef DEBUG
	long double value;
#endif
};
template <class IntType>
Rational<IntType> sqrt(const Rational<IntType>& rhs)
{
	return rhs.rsqrt();
}
template <class IntType>
Rational<IntType> abs(const Rational<IntType>& rhs)
{
	return rhs.rabs();
}
template <class IntType>
Rational<IntType> sign(const Rational<IntType>& rhs)
{
	return rhs.rsign();
}

template <class IntType>
Rational<IntType> stor(std::string input)
{
	IntType numerator = 0;
	IntType denominator = 1;
	boost::smatch results;
	boost::regex numberParse("(\\+|\\-)?(\\.)?([\\d]+)((\\.)([\\d]*))?");

	if (boost::regex_match(input, results, numberParse))
	{
		if (results[2].str() == ".")
		{
			IntType powerOfTen = (IntType)pow(10, results[3].str().size());
			numerator = std::stoi(results[3].str());
			denominator = powerOfTen;
		}
		else
		{
			numerator = std::stoi(results[3].str());

			if (results[6].str().size() > 0)
			{
				IntType powerOfTen = (IntType)pow(10, results[6].str().size());
				numerator *= powerOfTen;
				numerator += std::stoi(results[6]);
				denominator = powerOfTen;
			}
		}



		if (results[1].str() == "-")
			numerator *= -1;

		return Rational<IntType>(numerator, denominator);
	}
	throw std::invalid_argument("invalid string passed");
}
template <class IntType>
Rational<IntType> stor(const char* input)
{
	return stor<IntType>(std::string(input));
}