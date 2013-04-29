/*
 * File: EntropyGUID.java
 *
 * Entropy-GUID - Globally Unique IDentifier (GUID) entropy-based generator
 *   with 6 DOF
 * by Dionisis G. Kakoliris - dgk@bugsense.com
 *
 * 2012-05-11
 *
 * Java sample implementation
 */

/*
 * DISCLAIMER: Please note that the following sample implementation is just
 *   a proof of concept. One can refine it and produce better results.
 *   Especially the final digest step is implemented in a "naive" way for the
 *   sake of simplicity (using the MD5 algorithm - don't use this).
 */


//package dry;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Date;

public class EntropyGUID {
	private static final char[] DIGITS = { '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	private static char[] encodeHex(byte[] data) {
		int l = data.length;
		char[] out = new char[l << 1];

		// two characters form the hex value.
		for (int i = 0, j = 0; i < l; i++) {
			out[j++] = DIGITS[(0xF0 & data[i]) >>> 4];
			out[j++] = DIGITS[0x0F & data[i]];
		}

		return out;
	}

	public static void main(String[] args) {
		// STEP 1: get time since Epoch.
		// OUTPUT: timestamp in millisecond (or finer) granularity.
		// RATIONALE: collision only when app is deployed at the exact same
		//   time since Epoch (no local time).
		final String s1 = Long.toString((new Date()).getTime());

		// STEP 2: get an object reference.
		// OUTPUT: memory address or another distinguishable object property.
		// RATIONALE: very little probability for two runs to get the
		//   same address (debatable when in a sandbox).
		final String s2 = (new Object()).toString();

		// STEP 3: sleep for a little time.
		// OUTPUT: time delta in _finer_ than millisecond granularity.
		// RATIONALE: os fine timing granularity is not guaranteed
		//   (debatable when dealing with real-time/hard real-time
		//   kernels).
		final long dat1 = System.nanoTime();
		try {
			Thread.sleep(256, 42);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		final long dat2 = System.nanoTime();
		final String s3 = Long.toString(dat2 - dat1);

		// STEP 4: get a platform-dependent "distinguishable" context value.
		// OUTPUT: something different without needing escalated priviles.
		// RATIONALE: a value that may differentiate two contexts.
		//   Examples include: a device ID (without permissions for
		//   accessing it), number of physical processors,
		//   os version, file count of current directory,
		//   user name, number of processes spawned from the user, etc.
		final String s4 = Integer.toString(new File(".").list().length);

		// STEP 5: generate a random number.
		// OUTPUT: random integer.
		// RATIONALE: the pseudo-random generator may be the same, but there
		//   is a slight chance that something might have changed in the
		//   seed (may be a second passed in some cases).
		//   Also, some systems (e.g. a JVM) have their own pseudo-random
		//   generators that are "fed" with their own implementations,
		//   narrowing the probability for same results.
		final String s5 = Integer.toString((int) (Math.random() * 65535));

		// STEP 6: do a complementary step.
		// OUTPUT: the "age" of the implementation as a kick-starter.
		// RATIONALE: this is not a crucial step for the algorithm. We just
		//   provide an indication on the implementation "age". Examples
		//   include: an arbitrary string value, an integer,
		//   the publisher's name, etc.
		final String s6 = "www.bugsense.com::1";

		// The output string.
		final String sall = s1 + s2 + s3 + s4 + s5 + s6;
		System.out.println("str = " + sall);

		// MD5 from output string.
		byte[] thedigest = null;
		try {
			final byte[] bytesOfMessage = sall.getBytes("UTF-8");
			final MessageDigest md = MessageDigest.getInstance("MD5");
			thedigest = md.digest(bytesOfMessage);
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		}

		// RESULT.
		System.out.println("digest = " + new String(encodeHex(thedigest)));
	}
}


/* EOF */
