/*
 * File: EntropyGUID.cs
 *
 * Entropy-GUID - Globally Unique IDentifier (GUID) entropy-based generator
 *   with 6 DOF
 * by Dionisis G. Kakoliris - dgk@bugsense.com
 *
 * 2012-05-11
 *
 * C# sample implementation
 */

/*
 * DISCLAIMER: Please note that the following sample implementation is just
 *   a proof of concept. One can refine it and produce better results.
 *   Especially the final digest step is implemented in a "naive" way for the
 *   sake of simplicity (using the MD5 algorithm - don't use this).
 */


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Security.Cryptography;


namespace ConsoleApplication2
{
	class EntropyGUID
	{
		static void Main (string[] args)
		{
			// STEP 1: get time since Epoch.
			// OUTPUT: timestamp in millisecond (or finer) granularity.
			// RATIONALE: collision only when app is deployed at the exact same
			//   time since Epoch (no local time).
			String s1 = DateTime.Now.Millisecond.ToString ();

			// STEP 2: get an object reference.
			// OUTPUT: memory address or another distinguishable object property.
			// RATIONALE: very little probability for two runs to get the
			//   same address (debatable when in a sandbox).
			object o = new object ();
			string s2 = o.GetHashCode ().ToString ();

			// STEP 3: sleep for a little time.
			// OUTPUT: time delta in _finer_ than millisecond granularity.
			// RATIONALE: os fine timing granularity is not guaranteed
			//   (debatable when dealing with real-time/hard real-time
			//   kernels).
			DateTime dt = DateTime.Now;
			Thread.Sleep (256);
			DateTime dt2 = DateTime.Now;
			string s3 = (dt2.Ticks - dt.Ticks).ToString ();

			// STEP 4: get a platform-dependent "distinguishable" context value.
			// OUTPUT: something different without needing escalated priviles.
			// RATIONALE: a value that may differentiate two contexts.
			//   Examples include: a device ID (without permissions for
			//   accessing it), number of physical processors,
			//   os version, file count of current directory,
			//   user name, number of processes spawned from the user, etc.
			string s4 = System.IO.Directory.GetFiles (@".").Length.ToString ();

			// STEP 5: generate a random number.
			// OUTPUT: random integer.
			// RATIONALE: the pseudo-random generator may be the same, but there
			//   is a slight chance that something might have changed in the
			//   seed (may be a second passed in some cases).
			//   Also, some systems (e.g. a JVM) have their own pseudo-random
			//   generators that are "fed" with their own implementations,
			//   narrowing the probability for same results.
			string s5 = ((new Random ()).Next () % 65536).ToString ();

			// STEP 6: do a complementary step.
			// OUTPUT: the "age" of the implementation as a kick-starter.
			// RATIONALE: this is not a crucial step for the algorithm. We just
			//   provide an indication on the implementation "age". Examples
			//   include: an arbitrary string value, an integer,
			//   the publisher's name, etc.
			string s6 = "www.bugsense.com::1";

			// The output string.
			string sall = s1 + s2 + s3 + s4 + s5 + s6;
			Console.WriteLine ("str = " + sall);

			// MD5 from output string.
			using (MD5 md5Hash = MD5.Create()) {
				string hash = GetMd5Hash (md5Hash, sall);

				// RESULT.
				Console.WriteLine ("digest = " + hash);
			}
		}

		static string GetMd5Hash (MD5 md5Hash, string input)
		{

			// Convert the input string to a byte array and compute the hash.
			byte[] data = md5Hash.ComputeHash (Encoding.UTF8.GetBytes (input));

			// Create a new Stringbuilder to collect the bytes
			// and create a string.
			StringBuilder sBuilder = new StringBuilder ();

			// Loop through each byte of the hashed data
			// and format each one as a hexadecimal string.
			for (int i = 0; i < data.Length; i++) {
				sBuilder.Append (data [i].ToString ("x2"));
			}

			// Return the hexadecimal string.
			return sBuilder.ToString ();
		}
	}
}


/* EOF */
