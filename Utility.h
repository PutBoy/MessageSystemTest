#pragma once

namespace ECS
{
	class Noncopyable
	{
	public:
		Noncopyable() {};
	private:

		Noncopyable( const Noncopyable& other ) {}; // non construction-copyable
		Noncopyable& operator=( const Noncopyable& ) {}; // non copyable
	};

};