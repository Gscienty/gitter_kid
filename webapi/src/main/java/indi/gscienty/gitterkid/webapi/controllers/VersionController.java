package indi.gscienty.gitterkid.webapi.controllers;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping(value = { "/version" })
public class VersionController {
	
	@RequestMapping(value = { "/", "" }, method = RequestMethod.GET)
	public String getVersion() {
		return "GitterKid 0.1 (alpha)";
	}
}
